# Aperture-Duplicate-Finder

## Purpose

Find duplicate photos in your Aperture libraries.

## Why?

I have a huge Aperture library and a _lot_ of accidentally duplicated images (mainly due to importing iPhoto libraries). I need to organize my library but the shear number of duplicate images is making that a depressing chore. Other tools for finding duplicate images are slow and rather shit. Hopefully this tool will be faster and less shit.

## Requirements

* Mac OS X 10.7
* Apple Aperture (tested on version 3.1.3)
* Xcode 4

## Compilation

Before you try to compile the software read all of this README.

Don't forget to do a submodule update after cloning this repository.

## Usage

Before you try to use the software read all of this README.

Run the app, use the "Choose" button to select an Aperture library file (.aplibrary). Click "Start". The app will quit Aperture if it's already running, scan for duplicates, re-launch Aperture and then mark all duplicate images with the "x-duplicate" keyword.

Important!!! Make sure the Aperture library you want to process is the last Aperture library open before running this tool.

## Danger!!!

This tool reads files inside the aperture library bundle. This is not recommended by Apple. As far as I know the only legitimate way to access Aperture photos from a 3rd party app is via AppleScript - this has various issues, foremost of which is performance.

This tool is a work in progress. It might not compile, or might crash on running, parts of it may not be implemented yet.

## Works for me!!!

This tool works for me. It might work for you too. No guarantee or warranty is provided or implied. If it deletes or corrupts your Aperture library you have been warned.

## How it Works

The file finds all master images by reading the private and not-to-be-accessed-by-thirty-parties Library.apdb sqlite3 file. It then looks for all master images (files with a file extension that map to the public.image UTType).

It then enumerates over each image file, loading it into memory into a known image format (8-bit RGBA) and produces a MD5 hash for the pixel data. The hashes are then used to find duplicate images.

Each image that is a duplicate is then tagged as such with the keyword "x-duplicate".

## Performance

Much of the code has been made multiprocessor happy with the use of GCD. While running it should get close to 100% CPU for all the CPU cores in your system. On my 2.2GHz i7 Macbook PRO with 8 cores and 8GB of RAM and an SSD it can process about 800 images in less than a minute. It uses about 1.5GB of RAM while doing so and the machine is completely responsive. YMMV.

## Testing

Don't run this on your full Aperture library. That's nuts. Find an album or project that you know contains duplicates and then export that into a new library.

## BUGS/TODO

* Need to store the image digests so that we don't have to regenerate them for _all_ images _every_ run. This will allow the user to quit whenever he/she wants.
* Need to have a UI to show the duplicates and give the user the option to bail before we touch the library.
* Make sure i know what I'm doign with the version vs master stuff.

