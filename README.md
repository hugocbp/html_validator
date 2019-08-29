# HTML Validator

This is a program to validate a simple HTML subset file, developed as a class assignment for the CPSC 2150 Algorithms & Data Structures II class at Langara College.

## Instructions

1. Download or clone
2. ```make && ./htmlValidator```

## Requirements & Constraints

* Reads a filename from stdin and validates the file
* If file is invalid, display error and line number where the validation fails
* HTML subset: html | head | body | p | br | li | h1 | h2 | ul | ol
* Implement as ONE file (no extra header or implemenation files)

## Sample Output

Valid:

```
=============== HTML Validator ===============
Provided test files (suggested that some should be provided in class):
a.html (valid)
b.html (valid)
c.html (valid)
d.html (invalid - orphan closing tag)
e.html (invalid - wrong closing tags order)
f.html (invalid - tag not in grammar)
g.html (invalid - orphan closing tag)
h.html (invalid - orphan opening tag)
i.html (invalid - empty file)
z.html (invalid - non-existent file)

Enter the name of an html file in the current directory to validate: a.html
Validating a.html...
a.html is a valid HTML file according to the given grammar.
```

Invalid:

```
=============== HTML Validator ===============
Provided test files (suggested that some should be provided in class):
a.html (valid)
b.html (valid)
c.html (valid)
d.html (invalid - orphan closing tag)
e.html (invalid - wrong closing tags order)
f.html (invalid - tag not in grammar)
g.html (invalid - orphan closing tag)
h.html (invalid - orphan opening tag)
i.html (invalid - empty file)
z.html (invalid - non-existent file)

Enter the name of an html file in the current directory to validate: d.html
Validating d.html...

1: <html>
2:   <body>
3:     </h1>
       ^^^^^
[ERROR] Line 3: </h1> no matching opening tag. Should close <body>
```

```
=============== HTML Validator ===============
Provided test files (suggested that some should be provided in class):
a.html (valid)
b.html (valid)
c.html (valid)
d.html (invalid - orphan closing tag)
e.html (invalid - wrong closing tags order)
f.html (invalid - tag not in grammar)
g.html (invalid - orphan closing tag)
h.html (invalid - orphan opening tag)
i.html (invalid - empty file)
z.html (invalid - non-existent file)

Enter the name of an html file in the current directory to validate: f.html
Validating f.html...

1: <html>
2:   <head>
3:     <title>Test File</title>
       ^^^^^^^
[ERROR] Line 3: <title> is not a valid HTML tag (in the given grammar)
```


## Miscellaneous

All rights reserved. Don't use this code for your own assignments/labs.