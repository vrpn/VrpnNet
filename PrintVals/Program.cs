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
            Console.WriteLine("{0} {1} {2}", e.Time, e.Sensor, e.Position);
        }

        static void Main(string[] args)
        {
            TrackerRemote tracker = new TrackerRemote("Tracker0@localhost");
            tracker.PositionChanged += new TrackerChangeEventHandler(PositionChanged);
            tracker.MuteWarnings = true;

            while (true)
            {
                tracker.Update();
            }
        }
    }
}
