/*
crash.ck is designed to test / validate Max crash conditions
*/


//public class CustomEvent extends Event {}
class CustomEvent extends Event {}

global CustomEvent e;


while (true)
{
    1::second => now;
}
