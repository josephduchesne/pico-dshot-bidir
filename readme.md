<br/>
<p align="center">
  <h3 align="center">Bidirectional DShot for the rp2040</h3>

  <p align="center">
    Now with telemetry!
    <br/>
    <br/>
    <a href="https://github.com/josephduchesne/pico-dshot-bidir">View Demo</a>
    .
    <a href="https://github.com/josephduchesne/pico-dshot-bidir/issues">Report Bug</a>
    .
    <a href="https://github.com/josephduchesne/pico-dshot-bidir/issues">Request Feature</a>
  </p>
</p>

![Downloads](https://img.shields.io/github/downloads/josephduchesne/pico-dshot-bidir/total) ![Contributors](https://img.shields.io/github/contributors/josephduchesne/pico-dshot-bidir?color=dark-green) ![Forks](https://img.shields.io/github/forks/josephduchesne/pico-dshot-bidir?style=social) ![Stargazers](https://img.shields.io/github/stars/josephduchesne/pico-dshot-bidir?style=social) ![Issues](https://img.shields.io/github/issues/josephduchesne/pico-dshot-bidir) ![License](https://img.shields.io/github/license/josephduchesne/pico-dshot-bidir) 

## Table Of Contents

* [About the Project](#about-the-project)
* [Built With](#built-with)
* [Getting Started](#getting-started)
  * [Prerequisites](#prerequisites)
  * [Installation](#installation)
* [Usage](#usage)
* [Roadmap](#roadmap)
* [Contributing](#contributing)
* [License](#license)
* [Authors](#authors)
* [Acknowledgements](#acknowledgements)

## About The Project

Bidirectional DShot (DShot + telemetry on the same single wire) is tricky to implement without tying up valuable DMA channels or SPI interfaces. Thankfully, it is possible to offload most of that work onto the PIO state machines on the rp2040.

This library contains a variety of helper classes and PIO programs in a helpful bundle, delivered in the following formats:

|     **Format**     |  **Status** |
|:------------------:|:-----------:|
| PlatformIO Project | Working     |
| PlatformIO Library | In progress |
|    Arduino Library | Todo        |

# DShot Supported

| **Speed** | **Normal** | **Bidirectional**  |  Notes    |
|:---------:|:----------:|:------------------:|---------------|
| 150kBit   |  ✅        | ❌                 | not supported |
| 300kBit   |  ✅        | ✅                 |               |
| 600kBit   |  ✅        | ✅                 |               |
| 1200kBit  |  ✅        | ❌                 | *todo!*       |

## Built With



* [arduino-pico earlephilhower fork](https://github.com/earlephilhower/arduino-pico)
* [PlatformIO](https://platformio.org/)

## Getting Started


### Prerequisites

PlatformIO (TODO: Arduino IDE as well)

### Installation

1. Clone this repo
2. Open the folder in VSCode with the PlatformIO plugin extension installed
3. Wait for PlatformIO to install the core
4. Select one of the two environments (pi pico, or seediuno xiao 2040) or modify one to select a different rp2040 board.
5. Connect a DShot ESC to GPIO 6 (GP6 on the pi pico, D4 on the seeduino xiao). The ESC must be powered, have its ground tied to the rp2040 board and have a motor connected (please be careful!). Do not connect the motor's power to the rp2040 board or you will cook it, and possibly whatever computer it's connected to.
6. Press PlatfomIO: Upload. The board should flash, and the ESC should play 3 rising tones indicating that it's ready, followed by two rising tones indicating that it's armed

The example program does the following:
1. 3 seconds of DShot command 0 (stop). The ESC requires some amount of DShot command 0 (around 300ms) to arm.
2. 1 second of DShot command 13to enable extended telemetry (The ESC requires at least 6 consecutive commands to set this). This will do nothing if the ESC firmware doesn't support extended telemetry (BLHeli32 version 32.10 and newer support this).
3. DShot throttle 25% (note: if your ESC is configured for 3d mode this will be -50% throttle instead, use the setThrottle3D method instead of setThrottle to avoid this).

If the DShot::ESC instance is configured for DShot::Type::Bidir, telemetry data will be output on the usb serial terminal if it's being read successfully.

## Usage

TODO!

See docs: Also TODO! :/

## Roadmap

See the [open issues](https://github.com/josephduchesne/pico-dshot-bidir/issues) for a list of proposed features (and known issues).

## Contributing

Contributions are what make the open source community such an amazing place to be learn, inspire, and create. Any contributions you make are **greatly appreciated**.
* If you have suggestions for adding or removing projects, feel free to [open an issue](https://github.com/josephduchesne/pico-dshot-bidir/issues/new) to discuss it, or directly create a pull request after you edit the *README.md* file with necessary changes.
* Please make sure you check your spelling and grammar.
* Create individual PR for each suggestion.
* Please also read through the [Code Of Conduct](https://github.com/josephduchesne/pico-dshot-bidir/blob/main/CODE_OF_CONDUCT.md) before posting your first idea as well.

### Creating A Pull Request

1. Fork the Project
2. Create your Feature Branch (`git checkout -b feature/AmazingFeature`)
3. Commit your Changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the Branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

## License

Distributed under the MIT License. See [LICENSE](https://github.com/josephduchesne/pico-dshot-bidir/blob/main/LICENSE.md) for more information.

## Authors

* **Joseph Duchesne** - *Robot enthusiast* - [Joseph Duchesne](https://github.com/josephduchesne/) - *Added bidirectional support, refactored into library*
* **Connor Douthat** - *OG DShot+PIO hacker* - [Connor Douthat](https://github.com/cadouthat) - *DShot300 original implementation, DShot encoding functions*

## Acknowledgements

* [pico-dshot](https://github.com/cadouthat/pico-dshot) by Connor Douthat  - formed the basis of this project
* [DShot - The missing handbook](https://brushlesswhoop.com/dshot-and-bidirectional-dshot/) A vital guide to a murky specification.
* [arduino-pico](https://github.com/earlephilhower/arduino-pico), and specifically Earle F. Philhower, III  fork, which added PlatformIO support and many other enhancements
* [ShaanCoding - Readme generator](https://github.com/ShaanCoding/)
