
# Format

*Explanation of the structure with examples.*

<br>

## Comments

The graph format uses `/**/` multi-line comments.

```graph
/*
    A comment stretched over multiple lines
*/
```

<br>
<br>

## Nodes

Nodes group one of more items together under a name.

```graph
<Node Name> { <Node Content> }
```

<br>

### Name

-   Has to start with  `a - Z`
-   Can contain  `a - Z` ,  `0 - 9`  &  `_`

<br>

### Content

A nodes content - it's items - can either be direct  
values like numbers & strings or other nodes.

<br>

### Delimiters

To separate multiple items from one another,  
either use at least one **space** or **comma**.

```graph
NodeWithCommas { <ItemA> , <ItemB> }
NodeWithSpaces { <ItemA> <ItemB> }
```

<br>
<br>

## Numbers

Integers, as well as floats can be saved as is.

```graph
IntegerA { 10 }
IntegerB { -1 }
```

```graph
FloatA { 0.43 }
FloatB { -0.1 }
```

<br>
<br>

## Strings

Strings simply have to be wrapped in double quotes.

```graph
A_String { "Hello World" }
```

<br>

### Escaped Characters

Proceed as you usually would and prepend a backslash.

```graph
String_With_Escapes { "Hello \"World\"" }
```

<br>
<br>

## Mixed Structure

Is is also possible to mix direct values with other nodes.

```graph
Object {
    
    100 ,
    200 ,
    
    Data { 1 , 0 , 2 }
    
}
```

<br>
