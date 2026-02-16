`s = re.sub(r"[^a-z0-9_]+", " ", s)`

Replace anything that is NOT:
- lowercase letter a-z
- digit 0-9
- underscore _

with a space.

This matches characters such as:
- commas ,
- brackets [ ]
- semicolons ;
- plus +
- minus -
- colon :
- parentheses ( )
- etc.

## Example

Before:

`mov eax, [rbp-8]; add eax, 10`

After:

`mov eax rbp 8 add eax 10`

All punctuation is removed and replaced with spaces.

## Why this is useful?

Assembly contains many symbols:

`mov eax, [rbp-8]`

But similarity detection works better with clean tokens:

`mov eax rbp 8`

Remove noisy punctuation.

`s = re.sub(r"\s+", " ", s).strip()`


This fixes spacing.

`re.sub(r"\s+", " ", s)`


\s+ means:

one or more whitespace characters

Examples of whitespace:
- space
- tab
- newline

This converts multiple spaces into one space.

Before:

`mov   eax    rbp     8`


After:

`mov eax rbp 8`

## .strip()

Removes spaces from beginning and end.

Before:

`"  mov eax rbp 8  "`


After:

`"mov eax rbp 8"`

## Combined effect example

Without cleaning:
```
mov eax, [rbp-8]
mov eax,[rbp-8]
mov eax , [ rbp - 8 ]
```

These look different as raw text.

After cleaning:
```
mov eax rbp 8
mov eax rbp 8
mov eax rbp 8
```

Now they match.

Summary table
Line	Purpose
re.sub(r"[^a-z0-9_]+", " ", s)	remove punctuation and symbols
re.sub(r"\s+", " ", s)	collapse multiple spaces into one
.strip()	remove spaces at beginning/end
Final result

Turns messy assembly:

`mov eax, [rbp-8]; add eax, 10`


into clean tokens:

`mov eax rbp 8 add eax 10`


Which improves TF-IDF similarity quality.
