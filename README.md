# Sparse (Developer ReadMe)

# Overview

## Goals
The overall goal of this project is to create highly efficient and expandable  code to run several different population genetics proceses. At the moment, the main goal is to use sparcity along with other math/statistic/cs tricks to speed up processing related to a genotype matrx (Individuals X Snps). 

## Features (completed and in develpment)
- Unbiased heritability estimator
- Exact heriability calculator
- LD score calculator
- Genotype -> Phenotype simulator

## Installation

    git clone --recurse-submodules https://github.com/aldenlamp/Sparse.git
    cd Sparse
    make
This should create a `./sparse` executable in this folder. 

## Usage

You can run `./sparse -h` to see a full list of running options. Some examples include:
  - `./sparse -m_est_var` to calculate the estimated multi variance component analysis
  - `./sparse -s_ext_var` to calculate the exact single variance calculator
  - `./sparse -sim` to run a phenotype simulator

Each of the running options has their own set of required an optional arguments which can be seen using the `-h` tag. For example `./sparse -m_est_var -h` will show you that the following are required:
- `-g` a genotype file in either plink or vcf format (specified with `-vcf` tag)
- `-p` a phenotype file (accepted as a plink '.pheno')
- `-a` an annotation txt file

## Example test usage

Some test data is included in the `/data` folder which can be run as follows:

    ./sparse -m_est_var -g data/plink/small -p data/plink/small.pheno -a data/plink/annot.txt


# Code Overview

The entire project is split up into .hpp files (found in `/include`) and .cpp files (found in `/src`). Both of those folders have the same structure where each subfolder represents a different grouping of code functionality.

A high level overview of what each subfolder is responsible for:
- `arg_parse`: Everything required to parse arguments and generate config files to be used throughout
- `file_input`: Functionality to input genotype files in varying formats and output them to varying data structures
- `metadata`: Responsible for code metatdata (at the moment that soley involves internally timing code)
- `random_sampler`: A series of classes which helps handle random numbers
- `simulation`: Functionality responsible for simulating the phenotype and annotation data
- `snpdata`: Code responsible for different methods of storing and processing a genotype matrix
- `var_comp`: Funtionality for running variance component analysis

There are some loose files not in a folder, such as `vector_ops` and `matrix_solver` which are generally responsible for solving generic (static) problems that can be imported and used in a variety of places. While these exist here at the moment, throughout development, these files should either be removed or relocated to a fitting subfolder.

Below goes into more depth about the implementation and organization of all the code.

# Arg Parse

The goal of the arg parser is to allow an easy creation of (heirarctical) config files which automatically handle all the configs for you.

The three most important files to do this are `arg_parser`, `arg`, and `config`. 

## arg_parser
This is soley responsible for storing the inputted arguments and handling queries to check if a flag or a key value pair is the arguments. 

## arg

This file represents a single argument using the `Arg` class (as a template) and all the required info about it (such as name, keyword, description, required, etc.). One of the properies of this arg is a pointer to the corresponding variable in the config file. This means when the argument gets set (by passing in an `arg_parser`), it will automatically handle setting the value of the var in the config file. 

One other class found in this file is the `BaseArg` class. The `BaseArg` is a parent to the `Arg` without using templates the same way `Arg` does, but still abstracting all the important functions needed in `Arg`. This allows `Arg`s representing different types to all be stored inside a single vector and used while abstracting out the template aspect.

## config
All the config files that will be generated are formatted in a class heirarchy way where `config` is the parent of all. The main purpose of `config` is to store a vector of `Arg`s and run the important higher level processes (such as setting the values, checking if all required args are set, printing out the `-h` flag, etc.).

## organization of other configs
A good example of how the configs are inteded to be used can be seen with `genotype_file_config` and `var_calc_config`. 

`genotype_file_config`: Since inputting a genotype file is a relatively isolated process, it has its own config. In this config, it stores the variables needed such as a path to the file, the file format, and input limits. As seen in this file, the only thing needed to create a config, is adding the arguments you want and their corresponding variables. 

`var_calc_config`: This is the config responsible for running the variance calculator. Because running var calc requries a genotype file, this config inhertis from `genotype_file_config` meaning it adds all the arguments required to input a genotype file, and whenever a genotype file config is needed, it will be automatically upcasted to represent a genotype file config. (Just to make things slighlty easier, I added some extra code to differentiate between the different types of var comp analyses).

There is some code to determine if a config is a leaf config (the last child in the heriarchy) so when printing the `-h` flag, it will only print all the arguments once.

# Snp Data

The main goal of this folder (and namespace) is to manage the data corresponding to genotype files. Since there are different ways to store genotype data, there are different classes from each `SparseGenotypeData`, `DenseGenotypeData`, `BitwiseGenotypeData`. They all inherit from the parent `GenotypeData`. 

Originally, it was designed so the different files can be used for the same thing and the performace test against each other, but as development continues, it has become evident that each genotype data can do different processes with different levels of performace, thus `GenotypeData` has become more of an interface style inheritance (making all genotype files have a num of snps and a num of individuals) with factory classes for the different types. 

Each `GenotypeData` files inputs using a `genotype_file_config` which gets handled by the `file_input` section of the code.

Some of the `GenotypeData`s (Sparse and Bitwise) use their own genotype vectors to store the matrix as a single array of GentoypeVector objects. This means that the functionality for individual **snps** or **indidivuals** can be done using those vector classes. 

The different subclasses of `GenotypeData` have different groupings of functionality. For example, `SparseGenotypeData` has a functions to calculate XXt, to estimate the trace of the GRM and so forth. `BitwiseGenotypeData` has functionality to calculate a snp correlation matrix (XtX) and the LD score (in development). 

# File input
The goal of File Input is to isolate the functionality of inputting a genotype file from the disk. Each file represents a different file format to input from (at the moment there is only PLINK and VCF). But when it is used, generally the interface like parent `FileInput` class is used.

The file input is constructed from a `genotype_file_config` which will store what type of file format it uses (for a factory function that returns the correct upcasted derived class). 

After construction, the file input has functions to get the next `SNP` (row of genotype matrix) in whatever format is needed (`SparseSnpData`, `BitwiseGenotypeData`, or `vector<float>` for `DenseGenotypeData`). 

# Var Comp

This is the code responsible for variance component analysis. Each file represents a different way for example, single vs multiple components and estimate vs exact. Each uses a `GenotypeData` along with other inputs to run the analysis process. 

Most of the var comp files are responsible for calculating the variance components. Once they are done, there is a `result_values` file which takes the sigma values and calculates the heritabilities and enrichments or each component. It is also useful for ouputting in a nicely formatted way. 

# Simulation

The simulation part is responsible for taking a single genotype file (from a `genotype_file_config`) and generating simulated phenotype data and annotation data using the random sampler. It has functions both single and multi component analysis. 

# Random Sampler

There are a couple files here responsible for cleanly generating random numbers. One generates them from a discrete distribution, the other from a normal distribution.

# Metadata

As of now there is only a `Timer` file. The timer file is used throughout the code to time different functions and classes giving a high level overview of what is taking time (on top of callgrind/valgrinds analysis). 

Once a timer is created, is stores each different timed section as a start time and end time corresponding to a string representing the name of the timer. It also has a function to output to partitions (used for an external performance analyzer currently in development).

# Other files

There are two other important files. 

`matrix_solver`: This handles solving the final (small) matrix equation at the end of variance component analysis. It uses eigen's matrix solver for that. 

`vector_ops`: Becuase a lot of genotype data is saved as vectors of floats, vector_ops overrides a lot of mathematical vector functions such as addition, subtraction, multiplication, and summation. Even though it was useful for development, I believe that the way we currently use vector_ops addes overhead to the point where it would be useful to move away from using it and manually implementing vector math when needed. 