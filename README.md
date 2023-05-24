
# Handover Triggering Estimation based on Fuzzy Logic for LTE-A/5G Networks with Ultra Dense Small Cells

Increasing spectrum efficiency in new-generation communication networks is possible by raising the operating frequencies or increasing the number of serving cells. Both of the solutions, indirectly lead to a decrease in the cell size. By shrinking the coverage area of the cells, the mobility management becomes a critical issue in order to provide seamless connectivity. The main tool for mobility management in connected mode is the handover procedure, which conventionally is triggered based on the User Equipment's (UE's) measurements of serving and neighboring cells' signal quality. The performance of the methods provided in the 3GPP standard for setting up the handover, is based on a comparison between the quality of the received signal from the serving cell and the neighbor cells, which is severely sensitive to the appropriate setting of threshold values and waiting time for setting up the handover. The remarkable methods in literature to improve the handover performance in the 3GPP standard either utilize positional information or are based on complex algorithms. Positional information parameters such as speed, location, direction of movement and etc., need extra measurement modules. Also, complex algorithms like deep learning, are not suitable for a wide range of active devices in 5G networks with limited resources.
In this paper, a novel fuzzy logic-based method is proposed to trigger the handover procedure based on estimated serving cell's and neighbor cells' Radio Link Quality (RLQ) values. The proposed system consists of two stages. A second order regressor beside a simple fuzzy logic system is introduced to predict the serving cell's and neighbor cells' RLQ. The final handover trigger decision is made with the help of another cascade fuzzy logic system, which is responsible for eliminating too-early, too-late, and ping-pong handovers.
Considering the uncertainty handling feature of the interval type II fuzzy logic systems versus type I, we implement both methods and compare their results. Finally, we simulate the proposed algorithm using the ns-3 LTE module with a very tight setting for detecting radio link failure to meet 5G strict standards. Proposed method succeeded in improving the performance of the handover process at high-speed scenarios by 50%, only with the help of radio link quality information. The main advantage of the proposed method is the proper management of the handover procedure, independent of UE's velocity, as well as its simple structure with few rules, which makes it suitable for use in UAV and IoT devices.

# Fuzzy Model Integration for ns3 Simulation

The primary objective of this project is to integrate a fuzzy model into ns3 simulations. The fuzzy model is developed in Python using the PyIT2FLS package. To ensure an effective simulation, it is crucial to establish seamless data transmission between ns3 and Python while the ns3 project is running. In order to achieve this, we have introduced Python connection capabilities and integrated a set of header files into the ns3 framework.

## Implementation Details

The following files have been implemented to simulate and port ns3 data to Python:

1. **Proposed Scenario File:**
   - Location: `scratch/fuzzyH.cc`
   - Description: This file contains the proposed scenario for the ns3 simulation.

2. **Fuzzy Type I Algorithm (Implemented in Python):**
   - Location: `lte/model/fuzzy/type1fuzzy.py`
   - Description: This file implements the fuzzy Type I algorithm in Python.

3. **Fuzzy Type I Algorithm Interface Files:**
   - Location:
     - `lte/model/fuzzy-type1-handover-algorithm.h`
     - `lte/model/fuzzy-type1-handover-algorithm.cc`
   - Description: These files provide the interface for the fuzzy Type I algorithm implemented in Python.

4. **Fuzzy Type II Algorithm (Implemented in Python):**
   - Location: `lte/model/fuzzy/type2fuzzy.py`
   - Description: This file implements the fuzzy Type II algorithm in Python.

5. **Fuzzy Type II Algorithm Interface Files:**
   - Location:
     - `lte/model/fuzzy-type2-handover-algorithm.h`
     - `lte/model/fuzzy-type2-handover-algorithm.cc`
   - Description: These files provide the interface for the fuzzy Type II algorithm implemented in Python.

## Usage

To integrate the fuzzy model into ns3 simulations, follow these steps:

1. Copy the proposed scenario file (`fuzzyH.cc`) to the appropriate location in your ns3 project.
2. Import the fuzzy Type I and Type II algorithm files (`type1fuzzy.py` and `type2fuzzy.py`) into your Python project.
3. Include the fuzzy Type I and Type II algorithm interface files (`fuzzy-type1-handover-algorithm.h`, `fuzzy-type1-handover-algorithm.cc`, `fuzzy-type2-handover-algorithm.h`, `fuzzy-type2-handover-algorithm.cc`) in your ns3 project.
4. Configure the necessary parameters and functions to utilize the fuzzy model in your ns3 simulation.

## Citation

If you use this code or find our work helpful in your research, please consider citing the following paper:
[Handover Triggering Estimation based on Fuzzy Logic for LTE-A/5G Networks with Ultra Dense Small Cells](https://rdcu.be/dbxVC)

## Acknowledgments

We would like to express our gratitude to the developers of the PyIT2FLS package and the ns3 framework for their valuable contributions.

For any questions or inquiries, please contact us.


The Network Simulator, Version 3
================================

## Table of Contents:

1) [An overview](#an-open-source-project)
2) [Building ns-3](#building-ns-3)
3) [Running ns-3](#running-ns-3)
4) [Getting access to the ns-3 documentation](#getting-access-to-the-ns-3-documentation)
5) [Working with the development version of ns-3](#working-with-the-development-version-of-ns-3)

Note:  Much more substantial information about ns-3 can be found at
https://www.nsnam.org

## An Open Source project

ns-3 is a free open source project aiming to build a discrete-event
network simulator targeted for simulation research and education.
This is a collaborative project; we hope that
the missing pieces of the models we have not yet implemented
will be contributed by the community in an open collaboration
process.

The process of contributing to the ns-3 project varies with
the people involved, the amount of time they can invest
and the type of model they want to work on, but the current
process that the project tries to follow is described here:
https://www.nsnam.org/developers/contributing-code/

This README excerpts some details from a more extensive
tutorial that is maintained at:
https://www.nsnam.org/documentation/latest/

## Building ns-3

The code for the framework and the default models provided
by ns-3 is built as a set of libraries. User simulations
are expected to be written as simple programs that make
use of these ns-3 libraries.

To build the set of default libraries and the example
programs included in this package, you need to use the
tool 'waf'. Detailed information on how to use waf is
included in the file doc/build.txt

However, the real quick and dirty way to get started is to
type the command
```shell
./waf configure --enable-examples
```

followed by

```shell
./waf
```

in the directory which contains this README file. The files
built will be copied in the build/ directory.

The current codebase is expected to build and run on the
set of platforms listed in the [release notes](RELEASE_NOTES)
file.

Other platforms may or may not work: we welcome patches to
improve the portability of the code to these other platforms.

## Running ns-3

On recent Linux systems, once you have built ns-3 (with examples
enabled), it should be easy to run the sample programs with the
following command, such as:

```shell
./waf --run simple-global-routing
```

That program should generate a `simple-global-routing.tr` text
trace file and a set of `simple-global-routing-xx-xx.pcap` binary
pcap trace files, which can be read by `tcpdump -tt -r filename.pcap`
The program source can be found in the examples/routing directory.

## Getting access to the ns-3 documentation

Once you have verified that your build of ns-3 works by running
the simple-point-to-point example as outlined in 3) above, it is
quite likely that you will want to get started on reading
some ns-3 documentation.

All of that documentation should always be available from
the ns-3 website: https://www.nsnam.org/documentation/.

This documentation includes:

  - a tutorial

  - a reference manual

  - models in the ns-3 model library

  - a wiki for user-contributed tips: https://www.nsnam.org/wiki/

  - API documentation generated using doxygen: this is
    a reference manual, most likely not very well suited
    as introductory text:
    https://www.nsnam.org/doxygen/index.html

## Working with the development version of ns-3

If you want to download and use the development version of ns-3, you
need to use the tool `git`. A quick and dirty cheat sheet is included
in the manual, but reading through the git
tutorials found in the Internet is usually a good idea if you are not
familiar with it.

If you have successfully installed git, you can get
a copy of the development version with the following command:
```shell
git clone https://gitlab.com/nsnam/ns-3-dev.git
```

However, we recommend to follow the Gitlab guidelines for starters,
that includes creating a Gitlab account, forking the ns-3-dev project
under the new account's name, and then cloning the forked repository.
You can find more information in the [manual](https://www.nsnam.org/docs/manual/html/working-with-git.html).
