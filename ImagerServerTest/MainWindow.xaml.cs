using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
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
using Microsoft.Win32;
using Vrpn;

namespace ImagerServerSample
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        string serverName = "TestImage";
        Connection vrpnConnection;
        ImagerServer server;
        int width = 512;
        int height = 512;
        bool sendTestPattern = true;
        bool isColorPattern = false;
        bool isAlphaPattern = false;
        const double piO2 = Math.PI / 2.0;
        const double pi2 = Math.PI * 2.0;
        int frames = 0;
        DispatcherTimer timer;
        ImageBitDepth channelDepth = ImageBitDepth.unsigned8bit;

        public MainWindow()
        {
            InitializeComponent();

            vrpnConnection = Connection.CreateServerConnection();
            server = new ImagerServer(serverName, vrpnConnection, width, height);
            server.AddChannel("gray", ImageBitDepth.unsigned8bit);
            //server.ResendDescription();
            server.Update();
            vrpnConnection.Update();

            ServerNameLabel.Content = "The server name is: " + serverName;

            //Use a timer to update the server
            timer = new DispatcherTimer();
            timer.Tick += timer_Tick;
            timer.Interval = new TimeSpan(0, 0, 0, 0, 10);
            timer.Start();
        }

        void timer_Tick(object sender, EventArgs e)
        {
            if (server != null)
            {
                if (sendTestPattern)
                {
                    if (isColorPattern && isAlphaPattern)
                    {
                        if (channelDepth == ImageBitDepth.unsigned8bit)
                        {
                            byte[] image = CreateBGRA8TestImage(width, height, frames);
                            server.SendImage(0, 0, (ushort)(width - 1), 0, (ushort)(height - 1), 4, (uint)(width * 4), image, 2); //Send the blue channel
                            server.SendImage(1, 0, (ushort)(width - 1), 0, (ushort)(height - 1), 4, (uint)(width * 4), image, 1); //Send the green channel
                            server.SendImage(2, 0, (ushort)(width - 1), 0, (ushort)(height - 1), 4, (uint)(width * 4), image, 0); //Send the red channel
                            server.SendImage(3, 0, (ushort)(width - 1), 0, (ushort)(height - 1), 4, (uint)(width * 4), image, 3); //Send the alpha channel
                            frames++;
                        }
                        else if (channelDepth == ImageBitDepth.unsigned16bit)
                        {
                            UInt16[] image = CreateRGBA16TestImage(width, height, frames);
                            server.SendImage(0, 0, (ushort)(width - 1), 0, (ushort)(height - 1), 8, (uint)(width * 8), image, 0); //Send the blue channel
                            server.SendImage(1, 0, (ushort)(width - 1), 0, (ushort)(height - 1), 8, (uint)(width * 8), image, 2); //Send the green channel
                            server.SendImage(2, 0, (ushort)(width - 1), 0, (ushort)(height - 1), 8, (uint)(width * 8), image, 4); //Send the red channel
                            server.SendImage(3, 0, (ushort)(width - 1), 0, (ushort)(height - 1), 8, (uint)(width * 8), image, 6); //Send the alpha channel
                            frames++;
                        }
                        else
                        {
                            //We can also send using a array of the base type of the image
                            float[] image = CreateRGBA32TestImage(width, height, frames);
                            server.SendImage(0, 0, (ushort)(width - 1), 0, (ushort)(height - 1), 16, (uint)(width * 16), image, 0); //Send the red channel
                            server.SendImage(1, 0, (ushort)(width - 1), 0, (ushort)(height - 1), 16, (uint)(width * 16), image, 4); //Send the green channel
                            server.SendImage(2, 0, (ushort)(width - 1), 0, (ushort)(height - 1), 16, (uint)(width * 16), image, 8); //Send the blue channel
                            server.SendImage(3, 0, (ushort)(width - 1), 0, (ushort)(height - 1), 16, (uint)(width * 16), image, 12); //Send the alpha channel
                        }
                    }
                    else if (isColorPattern)
                    {
                        if (channelDepth == ImageBitDepth.unsigned8bit)
                        {
                            byte[] image = CreateRGB8TestImage(width, height, frames);
                            server.SendImage(0, 0, (ushort)(width - 1), 0, (ushort)(height - 1), 3, (uint)width * 3, image, 0); //Send the red channel
                            server.SendImage(1, 0, (ushort)(width - 1), 0, (ushort)(height - 1), 3, (uint)width * 3, image, 1); //Send the green channel
                            server.SendImage(2, 0, (ushort)(width - 1), 0, (ushort)(height - 1), 3, (uint)width * 3, image, 2); //Send the blue channel
                            frames++;
                        }
                        else if (channelDepth == ImageBitDepth.unsigned16bit)
                        {
                            //This one sends a 16-bit int stored in a byte array.  The server can figure out how to convert this based on the bit depth we gave it when we created the channel.
                            UInt16[] image = CreateRGB16TestImage(width, height, frames);
                            server.SendImage(0, 0, (ushort)(width - 1), 0, (ushort)(height - 1), 6, (uint)width * 6, image, 0); //Send the red channel
                            server.SendImage(1, 0, (ushort)(width - 1), 0, (ushort)(height - 1), 6, (uint)width * 6, image, 2); //Send the green channel
                            server.SendImage(2, 0, (ushort)(width - 1), 0, (ushort)(height - 1), 6, (uint)width * 6, image, 4); //Send the blue channel
                            frames++;
                        }
                    }
                    else
                    {
                        if (channelDepth == ImageBitDepth.unsigned8bit)
                        {
                            byte[] image = CreateGray8TestImage(width, height, frames);
                            bool success = server.SendImage(0, 0, (ushort)(width - 1), 0, (ushort)(height - 1), 1, (uint)width, image);
                            frames++;
                        }
                        else if (channelDepth == ImageBitDepth.unsigned16bit)
                        {
                            //This one sends a 16-bit int stored in a byte array.  The server can figure out how to convert this based on the bit depth we gave it when we created the channel.
                            byte[] image = CreateGray16TestImage(width, height, frames);
                            server.SendImage(0, 0, (ushort)(width - 1), 0, (ushort)(height - 1), 2, (uint)(width * 2), image);
                            //UInt16[] image = CreateGray16TestImage2(width, height, frames);
                            //server.SendImage(0, 0, (ushort)(width - 1), 0, (ushort)(height - 1), 2, (uint)(width * 2), image);
                            frames++;
                        }
                        else
                        {
                            //We can also send using a array of the base type of the image
                            //float[] image = CreateGray32TestImage(width, height, frames);
                            //server.SendImage(0, 0, (ushort)(width - 1), 0, (ushort)(height - 1), 4, (uint)(width * 4), image); //Notice that even though this uses a float array, the strides are still in bytes
                            byte[] image = CreateGray32TestImage2(width, height, frames);
                            server.SendImage(0, 0, (ushort)(width - 1), 0, (ushort)(height - 1), 4, (uint)(width * 4), image);
                            frames++;
                        }
                    }
                }

                server.Update();
                vrpnConnection.Update();
            }
        }

        //Send an image from a file
        private void sendImageButton_Click(object sender, RoutedEventArgs e)
        {
            OpenFileDialog openDlg = new OpenFileDialog();
            openDlg.Filter = "Image File (*.bmp, *.gif, *.jpg, *.png, *.tif)|*.bmp;*.gif;*.jpg;*.jpeg;*.png;*.tif;*.tiff";
            openDlg.Multiselect = false;

            if ((bool)openDlg.ShowDialog())
            {
                BitmapImage image = new BitmapImage(new Uri(openDlg.FileName));
                server.SetResolution(image.PixelWidth, image.PixelHeight);
                server.SendImageWPF((BitmapSource)image, 0, 1, 2);
                server.Update();
            }
        }

        #region Functions to generate test patterns
        //Creates an 8-bit grayscale test pattern
        byte[] CreateGray8TestImage(int width, int height, int offset)
        {
            byte[] data = new byte[width * height];
            for (int x = 0; x < width; x++)
            {
                for (int y = 0; y < height; y++)
                {
                    data[x + y * width] = (byte)((x + y + offset) % 256);
                }
            }

            return data;
        }
        //Creates a 16-bit grayscale test pattern
        byte[] CreateGray16TestImage(int width, int height, int offset)
        {
            byte[] data = new byte[width * height * 2];
            for (int x = 0; x < width; x++)
            {
                for (int y = 0; y < height; y++)
                {
                    UInt16 temp = (UInt16)Math.Abs(((x + y - offset) % 512) * 128);
                    byte[] tempBytes = BitConverter.GetBytes(temp);
                    data[(x + y * width) * 2 + 0] = tempBytes[0];
                    data[(x + y * width) * 2 + 1] = tempBytes[1];
                }
            }

            return data;
        }
        //Creates a 16-bit grayscale test pattern
        UInt16[] CreateGray16TestImage2(int width, int height, int offset)
        {
            UInt16[] data = new UInt16[width * height];
            for (int x = 0; x < width; x++)
            {
                for (int y = 0; y < height; y++)
                {
                    data[x + y * width] = (UInt16)Math.Abs(((x + y - offset) % 512) * 128);
                }
            }

            return data;
        }
        //Creates a 32-bit floating point grayscale test pattern
        float[] CreateGray32TestImage(int width, int height, int offset)
        {
            float[] data = new float[width * height];
            for (int x = 0; x < width; x++)
            {
                for (int y = 0; y < height; y++)
                {
                    //UInt16 temp = (UInt16)((x + y + offset) % 512);
                    //data[x + y * width] = ((float)(x + y + offset) / (float)width) % 1.0f;
                    data[x + y * width] = 0.5f;
                }
            }

            return data;
        }
        byte[] CreateGray32TestImage2(int width, int height, int offset)
        {
            byte[] data = new byte[width * height * 4];
            for (int x = 0; x < width; x++)
            {
                for (int y = 0; y < height; y++)
                {
                    float temp = 0.5f;
                    byte[] byteTemp = BitConverter.GetBytes(temp);
                    data[(x + y * width) * 4] = byteTemp[0];
                    data[(x + y * width) * 4 + 1] = byteTemp[1];
                    data[(x + y * width) * 4 + 2] = byteTemp[2];
                    data[(x + y * width) * 4 + 3] = byteTemp[3];
                }
            }

            return data;
        }
        //Creates an rgb, 8-bit per channel, test pattern
        byte[] CreateRGB8TestImage(int width, int height, double offset)
        {
            double radians = offset * Math.PI / 180.0;
            byte[] image = new byte[width * height * 3];
            double halfRad = Math.Min(width, height) / 2.0;

            for (int y = 0; y < height; y++)
            {
                for (int x = 0; x < width; x++)
                {
                    //Get x and y relative to the center of the image
                    double xCent = (double)x - (double)width / 2.0;
                    double yCent = (double)height / 2.0 - (double)y;

                    double r = radius(xCent, yCent);
                    r = r / halfRad;
                    double ang = angle(xCent, yCent);
                    ang = (ang + radians) % pi2 - Math.PI;
                    byte[] temp = getColor8(r, ang);
                    image[(y * width + x) * 3] = temp[0];
                    image[(y * width + x) * 3 + 1] = temp[1];
                    image[(y * width + x) * 3 + 2] = temp[2];
                }
            }

            return image;
        }
        UInt16[] CreateRGB16TestImage(int width, int height, double offset)
        {
            double radians = offset * Math.PI / 180.0;
            UInt16[] image = new UInt16[width * height * 3];
            double halfRad = Math.Min(width, height) / 2.0;

            for (int y = 0; y < height; y++)
            {
                for (int x = 0; x < width; x++)
                {
                    //Get x and y relative to the center of the image
                    double xCent = (double)x - (double)width / 2.0;
                    double yCent = (double)height / 2.0 - (double)y;

                    double r = radius(xCent, yCent);
                    r = r / halfRad;
                    double ang = angle(xCent, yCent);
                    ang = (ang + radians) % pi2 - Math.PI;
                    UInt16[] temp = getColor16(r, ang);
                    image[(y * width + x) * 3] = temp[0];
                    image[(y * width + x) * 3 + 1] = temp[1];
                    image[(y * width + x) * 3 + 2] = temp[2];
                }
            }

            return image;
        }
        byte[] CreateBGRA8TestImage(int width, int height, double offset)
        {
            double radians = offset * Math.PI / 180.0;
            byte[] image = new byte[width * height * 4];
            double halfRad = Math.Min(width, height) / 2.0;

            for (int y = 0; y < height; y++)
            {
                for (int x = 0; x < width; x++)
                {
                    //Get x and y relative to the center of the image
                    double xCent = (double)x - (double)width / 2.0;
                    double yCent = (double)height / 2.0 - (double)y;

                    double r = radius(xCent, yCent);
                    r = r / halfRad;
                    double ang = angle(xCent, yCent);
                    ang = (ang + radians) % pi2 - Math.PI;
                    byte[] temp = getColor8(r, ang);
                    image[(y * width + x) * 4] = temp[0];
                    image[(y * width + x) * 4 + 1] = temp[1];
                    image[(y * width + x) * 4 + 2] = temp[2];
                    image[(y * width + x) * 4 + 3] = (byte)clamp(((double)x/(double)width) * 255.0, 0, 255);
                }
            }

            return image;
        }
        UInt16[] CreateRGBA16TestImage(int width, int height, double offset)
        {
            double radians = offset * Math.PI / 180.0;
            UInt16[] image = new UInt16[width * height * 4];
            double halfRad = Math.Min(width, height) / 2.0;

            for (int y = 0; y < height; y++)
            {
                for (int x = 0; x < width; x++)
                {
                    //Get x and y relative to the center of the image
                    double xCent = (double)x - (double)width / 2.0;
                    double yCent = (double)height / 2.0 - (double)y;

                    double r = radius(xCent, yCent);
                    r = r / halfRad;
                    double ang = angle(xCent, yCent);
                    ang = (ang + radians) % pi2 - Math.PI;
                    UInt16[] temp = getColor16(r, ang);
                    image[(y * width + x) * 4] = temp[0];
                    image[(y * width + x) * 4 + 1] = temp[1];
                    image[(y * width + x) * 4 + 2] = temp[2];
                    image[(y * width + x) * 4 + 3] = (UInt16)clamp(((double)x / (double)width) * 65535.0, 0, 65535);

                    //image[(y * width + x) * 4] = 10000;
                    //image[(y * width + x) * 4 + 1] = 20000;
                    //image[(y * width + x) * 4 + 2] = 30000;
                    //image[(y * width + x) * 4 + 3] = 40000;
                }
            }

            return image;
        }
        float[] CreateRGBA32TestImage(int width, int height, double offset)
        {
            double radians = offset * Math.PI / 180.0;
            float[] image = new float[width * height * 4];
            double halfRad = Math.Min(width, height) / 2.0;

            for (int y = 0; y < height; y++)
            {
                for (int x = 0; x < width; x++)
                {
                    //Get x and y relative to the center of the image
                    double xCent = (double)x - (double)width / 2.0;
                    double yCent = (double)height / 2.0 - (double)y;

                    double r = radius(xCent, yCent);
                    r = r / halfRad;
                    double ang = angle(xCent, yCent);
                    ang = (ang + radians) % pi2 - Math.PI;
                    float[] temp = getColor32(r, ang);
                    //image[(y * width + x) * 4] = temp[0];
                    //image[(y * width + x) * 4 + 1] = temp[1];
                    //image[(y * width + x) * 4 + 2] = temp[2];
                    //image[(y * width + x) * 4 + 3] = 0.5f;

                    image[(y * width + x) * 4] = 0.75f;
                    image[(y * width + x) * 4 + 1] = 0.25f;
                    image[(y * width + x) * 4 + 2] = 1.0f;
                    image[(y * width + x) * 4 + 3] = 0.5f;
                }
            }

            return image;
        }

        //Math helpers
        double radius(double x, double y)
        {
            return Math.Sqrt(x * x + y * y);
        }
        double angle(double x, double y)
        {
            return Math.Atan2(y, x);
        }
        byte[] getColor8(double scaledRadius, double angle)
        {
            byte[] color = new byte[3];

            if (angle >= 0 && angle < piO2)
            {
                //color[0] = (byte)clamp(((piO2 - angle) / piO2) * scaledRadius * 255, 0, 255);
                color[0] = (byte)clamp(scaledRadius * 255, 0, 255);
                color[1] = (byte)clamp((angle / piO2) * scaledRadius * 255, 0, 255);
                color[2] = (byte)clamp((angle / piO2) * scaledRadius * 255, 0, 255);
            }
            else if (angle >= piO2)
            {
                color[0] = (byte)clamp(((2 * piO2 - angle) / piO2) * scaledRadius * 255, 0, 255);
                color[1] = (byte)clamp(((2 * piO2 - angle) / piO2) * scaledRadius * 255, 0, 255);
                color[2] = (byte)clamp(scaledRadius * 255, 0, 255);
            }
            else if (angle < 0 && angle >= -piO2)
            {
                color[0] = (byte)clamp(((piO2 + angle) / piO2) * scaledRadius * 255, 0, 255);
                color[1] = (byte)clamp((-angle / piO2) * scaledRadius * 255, 0, 255);
                color[2] = 0;
            }
            else
            {
                color[0] = 0;
                color[1] = (byte)clamp(((2 * piO2 + angle) / piO2) * scaledRadius * 255, 0, 255);
                color[2] = (byte)clamp(((-piO2 - angle) / piO2) * scaledRadius * 255, 0, 255);
            }

            return color;
        }
        UInt16[] getColor16(double scaledRadius, double angle)
        {
            UInt16[] color = new UInt16[3];

            if (angle >= 0 && angle < piO2)
            {
                //color[0] = (byte)clamp(((piO2 - angle) / piO2) * scaledRadius * 255, 0, 255);
                color[0] = (UInt16)clamp(scaledRadius * 65535, 0, 65535);
                color[1] = (UInt16)clamp((angle / piO2) * scaledRadius * 65535, 0, 65535);
                color[2] = (UInt16)clamp((angle / piO2) * scaledRadius * 65535, 0, 65535);
            }
            else if (angle >= piO2)
            {
                color[0] = (UInt16)clamp(((2 * piO2 - angle) / piO2) * scaledRadius * 65535, 0, 65535);
                color[1] = (UInt16)clamp(((2 * piO2 - angle) / piO2) * scaledRadius * 65535, 0, 65535);
                color[2] = (UInt16)clamp(scaledRadius * 65535, 0, 65535);
            }
            else if (angle < 0 && angle >= -piO2)
            {
                color[0] = (UInt16)clamp(((piO2 + angle) / piO2) * scaledRadius * 65535, 0, 65535);
                color[1] = (UInt16)clamp((-angle / piO2) * scaledRadius * 65535, 0, 65535);
                color[2] = 0;
            }
            else
            {
                color[0] = 0;
                color[1] = (UInt16)clamp(((2 * piO2 + angle) / piO2) * scaledRadius * 65535, 0, 65535);
                color[2] = (UInt16)clamp(((-piO2 - angle) / piO2) * scaledRadius * 65535, 0, 65535);
            }

            return color;
        }
        float[] getColor32(double scaledRadius, double angle)
        {
            float[] color = new float[3];

            if (angle >= 0 && angle < piO2)
            {
                //color[0] = (byte)clamp(((piO2 - angle) / piO2) * scaledRadius * 255, 0, 255);
                color[0] = (float)clamp(scaledRadius, 0, 1.0);
                color[1] = (float)clamp((angle / piO2) * scaledRadius, 0, 1.0);
                color[2] = (float)clamp((angle / piO2) * scaledRadius, 0, 1.0);
            }
            else if (angle >= piO2)
            {
                color[0] = (float)clamp(((2 * piO2 - angle) / piO2) * scaledRadius, 0, 1.0);
                color[1] = (float)clamp(((2 * piO2 - angle) / piO2) * scaledRadius, 0, 1.0);
                color[2] = (float)clamp(scaledRadius, 0, 1.0);
            }
            else if (angle < 0 && angle >= -piO2)
            {
                color[0] = (float)clamp(((piO2 + angle) / piO2) * scaledRadius, 0, 1.0);
                color[1] = (float)clamp((-angle / piO2) * scaledRadius, 0, 1.0);
                color[2] = 0;
            }
            else
            {
                color[0] = 0;
                color[1] = (float)clamp(((2 * piO2 + angle) / piO2) * scaledRadius, 0, 1.0);
                color[2] = (float)clamp(((-piO2 - angle) / piO2) * scaledRadius, 0, 1.0);
            }

            return color;
        }
        double clamp(double value, double min, double max)
        {
            if (value > max)
            {
                return max;
            }
            else if (value < min)
            {
                return min;
            }
            else
            {
                return value;
            }
        }
        #endregion

        #region Sanity checks on the image dimensions and send a new description if nessecary
        private void WidthTextBox_LostFocus(object sender, RoutedEventArgs e)
        {
            if (this.IsLoaded)
            {
                int tempWidth = width;

                if (int.TryParse(WidthTextBox.Text, out tempWidth))
                {
                    if (tempWidth > 0)
                    {
                        width = tempWidth;
                        server.SetResolution(width, height);
                    }
                    else
                    {
                        WidthTextBox.Text = width.ToString();
                    }
                }
                else
                {
                    WidthTextBox.Text = width.ToString();
                }
            }
        }
        private void HeightTextBox_LostFocus(object sender, RoutedEventArgs e)
        {
            if (this.IsLoaded)
            {
                int tempHeight = height;

                if (int.TryParse(HeightTextBox.Text, out tempHeight))
                {
                    if (tempHeight > 0)
                    {
                        height = tempHeight;
                        server.SetResolution(width, height);
                    }
                    else
                    {
                        HeightTextBox.Text = height.ToString();
                    }
                }
                else
                {
                    HeightTextBox.Text = height.ToString();
                }
            }
        }
        private void HeightTextBox_KeyUp(object sender, KeyEventArgs e)
        {
            if (this.IsLoaded)
            {
                if (e.Key == Key.Enter || e.Key == Key.Return)
                {
                    int tempHeight = height;

                    if (int.TryParse(HeightTextBox.Text, out tempHeight))
                    {
                        if (tempHeight > 0)
                        {
                            height = tempHeight;
                            server.SetResolution(width, height);
                        }
                        else
                        {
                            HeightTextBox.Text = height.ToString();
                        }
                    }
                    else
                    {
                        HeightTextBox.Text = height.ToString();
                    }
                }
            }
        }
        private void WidthTextBox_KeyUp(object sender, KeyEventArgs e)
        {
            if (this.IsLoaded)
            {
                if (e.Key == Key.Enter || e.Key == Key.Return)
                {
                    int tempWidth = width;

                    if (int.TryParse(WidthTextBox.Text, out tempWidth))
                    {
                        if (tempWidth > 0)
                        {
                            width = tempWidth;
                            server.SetResolution(width, height);
                        }
                        else
                        {
                            WidthTextBox.Text = width.ToString();
                        }
                    }
                    else
                    {
                        WidthTextBox.Text = width.ToString();
                    }
                }
            }
        }
        #endregion

        #region GUI stuff for selecting if a test pattern or a file are sent and update the server channels accordingly
        private void GrayTestRadioButton_Checked(object sender, RoutedEventArgs e)
        {
            if (this.IsLoaded)
            {
                sendTestPattern = true;
                isColorPattern = false;
                isAlphaPattern = false;
                sizeGroupBox.Visibility = System.Windows.Visibility.Visible;
                bitDepthGroupBox.Visibility = System.Windows.Visibility.Visible;
                _32bitRadioButton.IsEnabled = true;
                sendImageButton.Visibility = System.Windows.Visibility.Collapsed;
                ResetServer();
            }
        }
        private void rgbTestRadioButton_Checked(object sender, RoutedEventArgs e)
        {
            if (this.IsLoaded)
            {
                sendTestPattern = true;
                isColorPattern = true;
                isAlphaPattern = false;
                sizeGroupBox.Visibility = System.Windows.Visibility.Visible;
                bitDepthGroupBox.Visibility = System.Windows.Visibility.Visible;
                _32bitRadioButton.IsEnabled = false;
                if ((bool)_32bitRadioButton.IsChecked)
                {
                    _8bitRadioButton.IsChecked = true;
                }
                sendImageButton.Visibility = System.Windows.Visibility.Collapsed;
                ResetServer();
            }
        }
        private void rgbaTestRadioButton_Checked(object sender, RoutedEventArgs e)
        {
            if (this.IsLoaded)
            {
                sendTestPattern = true;
                isColorPattern = true;
                isAlphaPattern = true;
                sizeGroupBox.Visibility = System.Windows.Visibility.Visible;
                bitDepthGroupBox.Visibility = System.Windows.Visibility.Visible;
                _32bitRadioButton.IsEnabled = true;
                sendImageButton.Visibility = System.Windows.Visibility.Collapsed;
                ResetServer();
            }
        }
        private void fileRadioButton_Checked(object sender, RoutedEventArgs e)
        {
            if (this.IsLoaded)
            {
                sendTestPattern = false;
                isColorPattern = false;
                isAlphaPattern = false;
                sizeGroupBox.Visibility = System.Windows.Visibility.Collapsed;
                bitDepthGroupBox.Visibility = System.Windows.Visibility.Collapsed;
                _32bitRadioButton.IsEnabled = true;
                sendImageButton.Visibility = System.Windows.Visibility.Visible;
                ResetServer();
            }
        }
        private void _8bitRadioButton_Checked(object sender, RoutedEventArgs e)
        {
            if (this.IsLoaded)
            {
                channelDepth = ImageBitDepth.unsigned8bit;
                ResetServer();
            }
        }
        private void _16bitRadioButton_Checked(object sender, RoutedEventArgs e)
        {
            if (this.IsLoaded)
            {
                channelDepth = ImageBitDepth.unsigned16bit;
                ResetServer();
            }
        }
        private void _32bitRadioButton_Checked(object sender, RoutedEventArgs e)
        {
            if (this.IsLoaded)
            {
                channelDepth = ImageBitDepth.float32bit;
                ResetServer();
            }
        }
        #endregion

        //The server needs to be disposed manually, so let's cleanup before we close the window
        private void Window_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            sendImageButton.IsEnabled = false;
            timer.Stop();
            server.Dispose();
            vrpnConnection.Dispose();
        }

        //Reset the server to the new channel info if the properties are changed
        private void ResetServer()
        {
            if (server != null)
            {
                server.Dispose();
            }

            if (sendTestPattern)
            {
                if (isColorPattern && isAlphaPattern)
                {
                    server = new ImagerServer(serverName, vrpnConnection, width, height);
                    server.AddChannel("red", channelDepth);
                    server.AddChannel("green", channelDepth);
                    server.AddChannel("blue", channelDepth);
                    server.AddChannel("alpha", channelDepth);
                    server.SetResolution(width, height);
                    //server.ResendDescription();
                    server.Update();
                    vrpnConnection.Update();
                }
                else if (isColorPattern)
                {
                    server = new ImagerServer(serverName, vrpnConnection, width, height);
                    server.AddChannel("red", channelDepth);
                    server.AddChannel("green", channelDepth);
                    server.AddChannel("blue", channelDepth);
                    server.SetResolution(width, height);
                    //server.ResendDescription();
                    server.Update();
                    vrpnConnection.Update();
                }
                else
                {
                    server = new ImagerServer(serverName, vrpnConnection, width, height);
                    server.AddChannel("gray", channelDepth);
                    server.SetResolution(width, height);
                    //server.ResendDescription();
                    server.Update();
                    vrpnConnection.Update();
                }
            }
            else //Sending an image file will always use 8-bit RGB (it doesn't have to, we just will for simplicity)
            {
                server = new ImagerServer(serverName, vrpnConnection, width, height);
                server.AddChannel("red", ImageBitDepth.unsigned8bit);
                server.AddChannel("green", ImageBitDepth.unsigned8bit);
                server.AddChannel("blue", ImageBitDepth.unsigned8bit);
                server.SetResolution(width, height);
                //server.ResendDescription();
                server.Update();
                vrpnConnection.Update();
            }
        }
    }
}
