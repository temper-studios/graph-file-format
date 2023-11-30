# graph-file-format
This repo is a header only C program that loads and saves the .gf/.graph file format.
To get started, download the graph_file_format.h and include it in your C/C++ program. That's it!

Check out example.c for examples of usage. Check out the .graph files to see what the format looks like.

The graph file format is a plain text, hierarchical, data file format that is trivial to read and easy to parse.
For instance, the parser is only tens of lines long! It's simpler than JSON and as a bonus, can have comments.

If you want to simply serialise and deserialise data this could be an option for you.

It was designed primarily for games to save information about game state which is usually hierarchical.
Credit for the design goes to my Dad

The goals are
- Be as simple as possible.
- Be easy to parse.
- Be easy to read.
- Be frictionless.
- Be easy to follow the code.

The grammar is very simple and free form. Everything is a node and a node can be 4 different types: "composite", "floats", "integers" and "strings".

Nodes that have child nodes are called composite nodes. Below is a composite node called "node" with a single child node which is a value "1.0".
```
node { 1.0 } // node with a value child
```
Below is a composite node that has a child which is a composite node.
```
node { child { 2.0 } } // node with a child node
```
```
node { 1 2 3.0 "hello" child { -1.0 } } // node with a list of data including a child node
```
The delimiter between nodes can be a comma or a space. Nodes scope are ended with a closing brace
```
node { a, b } // a and b are two seperate nodes.
next { a } // next is seperate from node
```
Arrays are just value nodes that are next to each other.
```
matrix { 1 0 0 
         0 1 0 
         0 0 1 }
```
Multi line comments are allowed.
```
myFloat { 1.0 } /* this is a number */
```
Nested data structures are recommended!
```
Parent {
  Child {
      Child {
          "hello, world"
       }
   }  
}
```
Features:
- Saving and loading a C struct.
- Saving and loading anything really.
- Saving graphs or linked data structures.
- Can use a custom allocator so (admittedly not battle tested yet) can be used in an embedded environment.
- Send it over a network... or don't!
- Is fast. Potentially. Not measured it, but it doesn't do much and its written in C. Obvious optimisation can be done.
- Just looks pretty nice dunnit?
- It's not JSON.
- It's not XML.
- C is the first class citizen so can port anywhere! To any language! (but someone else should do that not me)
- You should use a binary format. But this is the next best thing.
