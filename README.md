# Valve Lighthouse 2.0 Tracker
### Northwestern Robotics Research Project 2020
#### by Bennett Espeland

## Objective
The goal of this research was to understand and recreate the tracking system implemented by Valve with the gen 2.0 lighthouses.

## Understanding
Lighthouse gen 1.0 (lh1) used a flash-sweep method to determine the position of a tracker in space.  However, gen 2.0 does not utilize this method, and instead uses encoded data in the sweep. This makes it more difficult to determine the position of the sensor as it is not as simple as finding the timing between flashes and sweeps.

The lh2 has a single rotor with two points that emit IR data.  This data is encoded using differential manchester (also known as Bipase Mark Code - BMC).  It contains the output from a Linear Feedback Shift Register (LFSR) which is determined using a polynomial represented by a 20-bit tap sequence.

Once data is captured on the device, it can be decoded and used to determine the polynomial being used by the lighthouse.  Once the polynomial is determined, the location of the data within a duplicate of the LFSR can be used to determine the angle where the sweep was detected.

## Result
Currently, the project is not in a fully working state.  It took a significant amount of time in order to configure the TS4231 correctly to work in LH2 mode.  After that, it was determining how to capture the data quickly enough (which was difficult/impossible for the MCUs I attempted).  The Adafruit Feather M0 (which was the only board I could get the TS4231 to config with), has the potential to work faster enought to capture the data.

It utilizes the event system with triggers from an edge detecting input capture.  The triggers starts a timer to count how long between detected edges, and then sends the result of that timer to memory with DMA.  Relying on the cpu to write to memory will not work as it is too slow.  Running the system at 48 Mhz achieved a timer count of 5-6 between 83ns pulses (the data is at 12 MHz).  

Currently, the decoders are not built to interpret live data.  That is in the testing phase and at the time of writing is not functioning.  They instead take data from the command line. (But when good live data is captured, it will be able to decode that.)


## Instillation Guide
###### Configuring TS4231
Follow the instructions for installing the TS4231 library.  You can then run the example included with the library through the arduino IDE to configure your chip.

###### Decoding .dat files
copy .dat file from gdrive
gcc .c files
cmd line: ./file.out DATA BASE

###### Running jdavidberger's code
run 'make' in the forked repo
then you can run the individual scripts


## Future Progress
- finalize DMAC config on feather for data capturing
- pass feather data to live decoders
- find sweep angles with poly + data + lfsr
- convert sweep angles to position

## Credits/Thanks
Thanks to cnlohr (Charles Lohr) for the reverse engineering streams on their youtube channel.  r2x0t and jdavidberger for their work in helping understand the signal coming from the trackers in the esptracker issue created by cnlohr.  And the people in the libsurvive discord for helping me find information and answering questions.