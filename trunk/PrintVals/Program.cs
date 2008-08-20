using System;
using System.Collections.Generic;
using System.Text;
using Vrpn;

namespace PrintVals
{
    class Program
    {
        static void PositionChanged(object sender, TrackerChangeEventArgs e)
        {
            Console.WriteLine("{0} {1} {2} {3} {4}", e.Time, e.Sensor, e.Position.X, e.Position.Y, e.Position.Z);
        }

        static void ButtonChanged(object sender, ButtonChangeEventArgs e)
        {
            Console.WriteLine("{0} {1} {2}", e.Time, e.Button, e.IsPressed ? "pressed" : "released");
        }

        static void Main(string[] args)
        {
            TrackerRemote tracker = new TrackerRemote("Tracker0@tracker1-cs.cs.unc.edu");
            tracker.PositionChanged += new TrackerChangeEventHandler(PositionChanged);
            tracker.MuteWarnings = false;

            ButtonRemote button = new ButtonRemote("Button0@tracker1-cs.cs.unc.edu");
            button.ButtonChanged += new ButtonChangeEventHandler(ButtonChanged);
            button.MuteWarnings = true;

            while (true)
            {
                tracker.Update();
                button.Update();
            }
        }
    }
}
