# strsepf

Single header string parsing function combining the memory safety of strsep and the convenience of a sscanf-like interface.

It is designed to be a safer `sscanf` for embedded system application.

Some key design choices:

- Built with C11.
- Will destroy its input string (by adding '\0').
- No dynamic memory allocation.
- Memory safe : no string copy, only pointer to the input buffer (not buffer overflow).
- No floating point support.
- Reentrant (RTOS safe).

Example:

```c
// Parse IP string
char                   myIP[] = "192.168.0.13";
char const* const ipStringFmt = "%3d.%3d.%3d.%3d";

uint32_t ip0 = 0;
uint32_t ip1 = 0;
uint32_t ip2 = 0;
uint32_t ip3 = 0;
int16_t  n = strsepf(myIP, ipStringFmt, &ip0, &ip1, &ip2, &ip3);

TEST_ASSERT_EQUAL(192, ip0);
TEST_ASSERT_EQUAL(168, ip1);
TEST_ASSERT_EQUAL(0,   ip2);
TEST_ASSERT_EQUAL(13,  ip3);
TEST_ASSERT_EQUAL(4,   n);
```

## Purpose

Let's say you have to write a C program to tokenize a string that contains a list of tokens separated by a space.

For example this : `version: alpha\n`. How do you extract information?


#### Option 1 : You could use `sscanf`

```c
char in[] = "version: alpha\n";

char versionName[12] = {0};
sscanf(in, "%*s %s\n", versionName);

TEST_ASSERT_EQUAL_STRING("alpha", versionName); //< This will pass.
```

While this is certainly readable and compact, some problem can be encountered. For example a string bigger than 11 will provoke a buffer overflow.
You can read a blog post about the danger of `scanf` [here](http://sekrit.de/webdocs/c/beginners-guide-away-from-scanf.html).

#### Option 2: You could use `sscanf` in a saffer way

```c
char in[] = "version: alpha\n";
char versionName[12] = {0};

if (sscanf(in, "%*s %11[^\n]%*c", versionName) != 1)
{
    return -1; // We expect exactly 1 conversion
}
```

While this option more secure, this is less readable and now the buffer size is hardcoded in the format (less maintenable).


#### Option 3: strtok

`strtok` is not a good idea.

It uses a static buffer while parsing, so it's not reentrant (not good for RTOS).
It does not correctly handle "empty" fields -- that is, where two delimiters are back-to-back and meant to denote the lack of information in that field.

#### Option 4: strtok_r

`strtok_r` is the reentrant version of `strtok`.

```c
char in[] = "version: alpha\n";

char *saveptr;
char *token = strtok_r(str, " ", &saveptr); //< skip `version: `
if (token == NULL){
    return -1;
}

//                           v Note the NULL here.
char *versionName = strtok_r(NULL, "\n", &saveptr); //< get `alpha`
if (versionName == NULL){
    return -1;
}
```

This is a good solution, but the interface is less than beautiful and more verbose.

This is especially true if we want to parse number in a string.

#### Option 5: strsep

`strsep` is a BSD function. Therefore, there's no garantiy it will be included in `string.h`.

But it's an alternative to `strtok_r` with a better interface.

```c
char in[] = "version: alpha\n";

char *str = in;
char *token = strsep(&str, " "); //< skip `version: `
if (token == NULL){
    return -1;
}

char *versionName  = strsep(&str, "\n"); //< get `alpha`
if (versionName == NULL){
    return -1;
}
```

This is a better solution, but the interface is still quite verbose, especially if we want to extract numbers from a string.

#### Option 6: using `strsepf`

Simple usage:

```c
char in[] = "version: alpha\n";

char *versionName = NULL;
strsepf(in, "%*s %s\n", &versionName);
if (versionName == NULL){
    return -1;
}
```

## Building

This was built with gcc 7.4.0 on WSL.

```sh
cd build
cmake ..
make
```

## Testing

This was built with gcc 7.4.0 on WSL.

```sh
# Install unit test framework
cd external
git clone https://github.com/ThrowTheSwitch/Unity.git

# Build
cd ..
cd build
cmake ..
make

# Run tests
./run
```

## License

MIT License - Copyright (c) 2019 G. Berthiaume

See the `LICENCE` file for more information.
