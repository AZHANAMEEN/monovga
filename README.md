# monovga

This is a simple program for the avr64da chips (though it should also work on other dx chips with some changes) for generating VGA video or static images it uses the event system of these chips to communicate between timers TCD0 and TCA0 for hsync and vsync. it locks onto monitors for now (tested on dell E1916hv), picture is yet to be done.

# Progress

currently works on 640x480 res and locks on sucesfully
<img width="1600" height="725" alt="WhatsApp Image 2026-10-10 at 1 35 45 PM" src="https://github.com/user-attachments/assets/03dd539d-d8b6-41c8-84c6-09c60e4bcd02" />

# usage


you can simply open the test.ino file and upload it via a updi programmer or if you don't have one, you can easily find how to make one from a usb-ttl adapter , and just upload this code and connect your hsync pin to PA4 and vstync to PA0, it is reccomended to use a vga breakout board as direct wires can mess up your signals. 
