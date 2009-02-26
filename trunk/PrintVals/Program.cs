// Program.cs: Sample printvals-style app demonstrating VrpnNet functionality. 
//
// Copyright (c) 2008-2009 Chris VanderKnyff
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

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
            Console.WriteLine("{0} {1} {2}", e.Time.ToLocalTime(), e.Sensor, e.Position);
        }

        static void ButtonChanged(object sender, ButtonChangeEventArgs e)
        {
            Console.WriteLine("{0} {1} {2}", e.Time, e.Button, e.IsPressed ? "pressed" : "released");
        }

        static void Main(string[] args)
        {
            TrackerRemote tracker = new TrackerRemote("Tracker0@localhost");
            tracker.PositionChanged += new TrackerChangeEventHandler(PositionChanged);
            tracker.MuteWarnings = false;

            ButtonRemote button = new ButtonRemote("Button0@localhost");
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
