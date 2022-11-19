
<br>

<div align = center>

# Graph File Format

A text format used to save & load hierarchical data from  
`.graph`  files, primarily designed for saving game states.

<br>

[![Button Example]][Example]  
[![Button Format]][Format]  
[![Button Video]][Video]

<br>
<br>

***Credit for the design goes to my Dad.***

<br>
<br>

## Goals

<kbd> <br> Simple as Possible <br> </kbd>  
<kbd> <br> Frictionless <br> </kbd>  
<kbd> <br> Easy to Read / Parse <br> </kbd>


<br>
<br>

## Format Example

*A small example of how you can structure data in graph files.*

</div>

<br>

```graph
/* An example of the .graph file format */

Model {
    
    Id { "Basic Component" }
    
    Shape {
        0  0  50 10  70 40
       84 31   6 12   1 23
    }
    
    Density { 0.4 }
    
    Position {
        X { -4 }
        Y {  3 }
        z { 10 }
    }
}
```

<br>


<!----------------------------------------------------------------------------->

[Example]: example.c
[Format]: Documentation/Format.md
[Video]: https://www.youtube.com/watch?v=wG0gW6viPJs


<!---------------------------------[ Buttons ]--------------------------------->

[Button Example]: https://img.shields.io/badge/Example-00B388?style=for-the-badge&logoColor=white&logo=VisualStudioCode
[Button Format]: https://img.shields.io/badge/Format-008FC7?style=for-the-badge&logoColor=white&logo=BookStack
[Button Video]: https://img.shields.io/badge/Introduction-CA4245?style=for-the-badge&logoColor=white&logo=YouTube
