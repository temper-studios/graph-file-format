
# Format


<br>




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