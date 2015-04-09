using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Windows.Threading;
using Vrpn;

namespace ImagerRemoteSample
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        ImagerRemote imager;
        DispatcherTimer timer;
        PixelFormat imageFormat;
        byte[] rawImage;
        int width;
        int height;
        int depth;
        WriteableBitmap image;
        int bytesPerPixel = 0;
        int bytesPerChannel = 0;
        string serverName = "TestImage@localhost";
        bool useChannelBinding = false;

        System.Diagnostics.Process thisProc;

        public MainWindow()
        {
            InitializeComponent();
            
            //Connect to the server and subscribe to the events we want
            imager = new ImagerRemote(serverName);
            imager.DescriptionReceived += new DescriptionReceivedEventHandler(imager_DescriptionReceived);
            imager.FramesDiscarded += new FramesDiscardedEventHandler(imager_FramesDiscarded);
            imager.FrameReceived += new FrameReceivedEventHandler(imager_FrameReceived);
            imager.MuteWarnings = true;
            imager.UnbindChannels(); //This isn't required here, but it doesn't hurt anything

            //Put the name of the server on the GUI
            ServerNameLabel.Content = "Listening to server: " + serverName;

            //Use a timer to keep the VRPN connection updated
            timer = new DispatcherTimer();
            timer.Tick += new EventHandler(timer_Tick);
            timer.Interval = new TimeSpan(0, 0, 0, 0, 15);
            timer.Start();

            thisProc = System.Diagnostics.Process.GetCurrentProcess();
            thisProc.ErrorDataReceived += thisProc_ErrorDataReceived;
            thisProc.OutputDataReceived += thisProc_ErrorDataReceived;
        }

        void thisProc_ErrorDataReceived(object sender, System.Diagnostics.DataReceivedEventArgs e)
        {
            //System.Diagnostics.Debug.Write(e.Data);
        }

        //Display the received image on screen
        void imager_FrameReceived(object sender, FrameReceivedEventArgs e)
        {
            if (e.IsBoundFrame && useChannelBinding) //We only want to render a bound frame, this will prevent any stray unbound frames from crashing the code
            {
                image.WritePixels(new Int32Rect(0, 0, width, height), e.Image, width * bytesPerPixel, 0);
                if (imageFormat == PixelFormats.Rgba128Float)
                {
                    float[] temp = new float[width * height * 4];
                    for (int i = 0; i < width * height * 4; i++)
                    {
                        temp[i] = BitConverter.ToSingle(e.Image, i * 4);
                    }
                }
            }
            else if (!useChannelBinding && !e.IsBoundFrame)
            {
                //With channel binding turned off we have to compile the color frame manually
                
                //This is easy for the grayscale case
                if (imageFormat == PixelFormats.Gray8 || imageFormat == PixelFormats.Gray16 || imageFormat == PixelFormats.Gray32Float)
                {
                    rawImage = e.Image;
                }
                else if (imageFormat == PixelFormats.Rgb24 || imageFormat == PixelFormats.Rgb48)
                {
                    int channelOffset = 0;
                    
                    //Get the offset of the channel based on its index.  Similiar to the binding, in a real application, this would probably be done by name not index
                    for (int i = 0; i < imager.Channels.Length; i++)
                    {
                        if (imager.Channels[i].Name == e.Channel)
                        {
                            channelOffset = i * (bytesPerPixel / 3);
                        }
                    }

                    for (int i = 0; i < (e.Image.Length / bytesPerChannel); i++)
                    {
                        for (int j = 0; j < bytesPerChannel; j++)
                        {
                            rawImage[i * bytesPerPixel + channelOffset + j] = e.Image[i * bytesPerChannel + j];
                        }
                    }
                }
                else if (imageFormat == PixelFormats.Rgba64 || imageFormat == PixelFormats.Rgba128Float)
                {
                    int channelOffset = 0;

                    //Get the offset of the channel based on its index.  Similiar to the binding, in a real application, this would probably be done by name not index
                    for (int i = 0; i < imager.Channels.Length; i++)
                    {
                        if (imager.Channels[i].Name == e.Channel)
                        {
                            channelOffset = i * (bytesPerPixel / 4);
                        }
                    }

                    for (int i = 0; i < (e.Image.Length / bytesPerChannel); i++)
                    {
                        for (int j = 0; j < bytesPerChannel; j++)
                        {
                            rawImage[i * bytesPerPixel + channelOffset + j] = e.Image[i * bytesPerChannel + j];
                        }
                    }
                }
                else if (imageFormat == PixelFormats.Bgra32)
                {
                    int channelOffset = 0;

                    //Get the offset of the channel based on its index.  Similiar to the binding, in a real application, this would probably be done by name not index
                    for (int i = 0; i < imager.Channels.Length; i++)
                    {
                        if (imager.Channels[i].Name == e.Channel)
                        {
                            //Again, for BGRA we have to swap the order since we are assuming RGBA in the channel order
                            if (i == 0)
                            {
                                channelOffset = 2 * (bytesPerPixel / 4);
                            }
                            else if (i == 2)
                            {
                                channelOffset = 0;
                            }
                            else
                            {
                                channelOffset = i * (bytesPerPixel / 4);
                            }
                        }
                    }

                    for (int i = 0; i < e.Image.Length; i++)
                    {
                        rawImage[i * bytesPerPixel + channelOffset] = e.Image[i];
                    }
                }

                //Finally, we can write the raw image to the display.  As you can see, channel binding does a lot of the work for you.
                image.WritePixels(new Int32Rect(0, 0, width, height), rawImage, width * bytesPerPixel, 0);
            }
        }

        //Write a message if any frames are discarded
        void imager_FramesDiscarded(object sender, FramesDiscardedEventArgs e)
        {
            ConsoleTextBox.Text += "Frames Discarded: " + e.Time.ToString() + " Count: " + e.DiscardedCount + "\r\n";
        }

        void timer_Tick(object sender, EventArgs e)
        {
            imager.Update();
        }

        //Update the display parameters based on the image description
        void imager_DescriptionReceived(object sender, DescriptionReceivedEventArgs e)
        {
            imager.UnbindChannels(); //Unbind the channels so we can set the new channel bindings base on the new description

            //Save the size of the image to class variables so we can use it later
            width = e.Columns;
            height = e.Rows;
            depth = e.Depth;

            //Setup the colors
            if (imager.Channels.Length == 1)
            {
                //Grayscale doesn't require channel binding
                if (imager.Channels[0].BitDepth == ImageBitDepth.unsigned8bit)
                {
                    imageFormat = PixelFormats.Gray8;
                    bytesPerPixel = 1;
                    bytesPerChannel = 1;
                }
                else if (imager.Channels[0].BitDepth == ImageBitDepth.float32bit)
                {
                    imageFormat = PixelFormats.Gray32Float;
                    bytesPerPixel = 4;
                    bytesPerChannel = 4;
                }
                else
                {
                    imageFormat = PixelFormats.Gray16;
                    bytesPerPixel = 2;
                    bytesPerChannel = 2;
                }
            }
            else if (imager.Channels.Length == 3)
            {
                //For RGB, we will bind the first 3 channels
                //In a real application, it would be preferable to bind by channel name
                if (imager.Channels[0].BitDepth == ImageBitDepth.unsigned8bit)
                {
                    imageFormat = PixelFormats.Rgb24;
                    bytesPerPixel = 3;
                    bytesPerChannel = 1;
                    if (useChannelBinding)
                    {
                        imager.BindChannels("BoundVirtualChannel", imager.Channels[0].Name, 0, imager.Channels[1].Name, 1, imager.Channels[2].Name, 2);
                    }
                }
                else if (imager.Channels[0].BitDepth == ImageBitDepth.float32bit)
                {
                    //WPF doesn't support a 96 bits-per-pixel RGB format
                    throw new Exception("Format not supported.");
                }
                else
                {
                    imageFormat = PixelFormats.Rgb48;
                    bytesPerPixel = 6;
                    bytesPerChannel = 2;
                    if (useChannelBinding)
                    {
                        imager.BindChannels("BoundVirtualChannel", imager.Channels[0].Name, 0, imager.Channels[1].Name, 2, imager.Channels[2].Name, 4);
                    }
                }
            }
            else if (imager.Channels.Length == 4)
            {
                //Similiarly, for RGBA we will bind the first 4 channels
                //In a real application, it would be preferable to bind by channel name
                if (imager.Channels[0].BitDepth == ImageBitDepth.unsigned8bit)
                {
                    //Since this format is bgra instead of rgba, we reverse the order of the channels assuming blue will be the 3rd channel and red the 1st
                    imageFormat = PixelFormats.Bgra32;
                    bytesPerPixel = 4;
                    bytesPerChannel = 1;
                    if (useChannelBinding)
                    {
                        imager.BindChannels("BoundVirtualChannel", imager.Channels[2].Name, 0, imager.Channels[1].Name, 1, imager.Channels[0].Name, 2, imager.Channels[3].Name, 3);
                    }
                }
                else if (imager.Channels[0].BitDepth == ImageBitDepth.float32bit)
                {
                    imageFormat = PixelFormats.Rgba128Float;
                    bytesPerPixel = 16;
                    bytesPerChannel = 4;
                    if (useChannelBinding)
                    {
                        imager.BindChannels("BoundVirtualChannel", imager.Channels[0].Name, 0, imager.Channels[1].Name, 4, imager.Channels[2].Name, 8, imager.Channels[3].Name, 12);
                    }
                }
                else
                {
                    imageFormat = PixelFormats.Rgba64;
                    bytesPerPixel = 8;
                    bytesPerChannel = 2;
                    if (useChannelBinding)
                    {
                        imager.BindChannels("BoundVirtualChannel", imager.Channels[0].Name, 0, imager.Channels[1].Name, 2, imager.Channels[2].Name, 4, imager.Channels[3].Name, 6);
                    }
                }
            }

            if (bytesPerPixel != 0)
            {
                image = new WriteableBitmap(width, height, 96.0, 96.0, imageFormat, null);
                DisplayedImage.Source = image;
                rawImage = new byte[e.Rows * e.Columns * e.Depth * bytesPerPixel];

                ConsoleTextBox.Text += String.Format("New description Received at: {0}, the new size is {1} x {2} x {3} with a format of " + imageFormat.ToString() + "\r\n", e.Time, e.Rows, e.Columns, e.Depth);
            }
        }

        //Turn channel binding on and off.  This is only shown for demonstrative purposes, most applications will want to leave it on or off the entire time
        private void channelBindCheckBox_Click(object sender, RoutedEventArgs e)
        {
            if ((bool)channelBindCheckBox.IsChecked && !useChannelBinding)
            {
                bool success = false;
                if (imageFormat == PixelFormats.Rgb24)
                {
                    success = imager.BindChannels("BoundVirtualChannel", imager.Channels[0].Name, 0, imager.Channels[1].Name, 1, imager.Channels[2].Name, 2);
                }
                else if (imageFormat == PixelFormats.Rgb48)
                {
                    success = imager.BindChannels("BoundVirtualChannel", imager.Channels[0].Name, 0, imager.Channels[1].Name, 2, imager.Channels[2].Name, 4);
                }
                else if (imageFormat == PixelFormats.Bgra32)
                {
                    success = imager.BindChannels("BoundVirtualChannel", imager.Channels[2].Name, 0, imager.Channels[1].Name, 1, imager.Channels[0].Name, 2, imager.Channels[3].Name, 3);
                }
                else if (imageFormat == PixelFormats.Rgba64)
                {
                    success = imager.BindChannels("BoundVirtualChannel", imager.Channels[0].Name, 0, imager.Channels[1].Name, 2, imager.Channels[2].Name, 4, imager.Channels[3].Name, 6);
                }
                else if (imageFormat == PixelFormats.Rgba128Float)
                {
                    success = imager.BindChannels("BoundVirtualChannel", imager.Channels[0].Name, 0, imager.Channels[1].Name, 4, imager.Channels[2].Name, 8, imager.Channels[3].Name, 12);
                }

                if (success)
                {
                    useChannelBinding = true;
                }
                else
                {
                    channelBindCheckBox.IsChecked = false;
                    ConsoleTextBox.Text += "Channel binding is not available with the current image format.\r\n";
                }
            }
            else if (!((bool)channelBindCheckBox.IsChecked) && useChannelBinding)
            {
                imager.UnbindChannels();
                useChannelBinding = false;
            }
        }
    }
}
