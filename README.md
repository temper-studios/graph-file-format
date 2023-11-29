# graph-file-format
The .graph file format is a plain text file format used to save and load hierarchical data.
It was designed primarily for games to save information about game state which is usually
hierarchical.
Credit for the design goes to my Dad

The goals are
- Be as simple as possible.
- Be easy to parse.
- Be easy to read.
- Be frictionless

Check out example.c for examples.

The goal is to express hierarchical data structures in the most simple way possible.
Nodes can have children. Those child nodes can be nodes with a value (like a number) or 
more nodes. e.g.
```
node { 1.0 } // node with a value child
```
```
node { child { 2.0 } } // node with a child node
```
```
node { 1 2 3.0 "hello" child { -1.0 } } // node with a list of data including a child node
```
The delimiter between nodes is a space. Nodes scope are ended with a closing brace
```
node { a b } // a and b are two seperate nodes.
next { a } // next is seperate from node
```
You can do matrices 
```
matrix { 1 0 0 
         0 1 0 
         0 0 1 }
```
You can do comments 
```
myFloat { 1.0 } // this is a number
```
You can do nested data structures 
```
Parent {
  Child {
      Child {
          "hello, world"
       }
   }  
}
```
