# Make A Date C Implementation README

This implementation specifically is mostly in the standard C library, the only external library it uses is [LINK TO THE JSON PARSER]. To build the assembled code run download the library and you can compile with the following command line [INSERT COMMAND LINE].

Once compiled, you as the user have various ways of using the application. The two main ways are reading and writing to the JSON file.

--- 
## Operations
### Reading
To read the JSON file stored in the same directory as the compiled code run the following command (it is case insensitive):
``` bash
./NAME read
```
This will read the entire JSON file in its intierty, and print out every piece of information as follows...

[IMAGE of ./MAD-V1 read]



### Filtering
To specify if you would like to **F**ilter **i**n or **F**ilter **o**ut, you can use the arguments `—fi` or `—fo` **but not both!** These allow you to select what columns you would like to see.

``` bash
    ./NAME -fi Idea daTE TyPe
```
[IMAGE-OF-./MAD-V1--fi-Idea-daTE-TyPe]

``` bash
    ./NAME -fo Idea daTE TyPe
```
[IMAGE-OF-./MAD-V1--fo-Idea-daTE-TyPe]


They take a list of column names, in any order and any case, as their arguments seperated by spaces e.g `name date type`. Any columns not found, i.e arguments with an invalid name, will not throw an error. The program will run and print the result but will warn the uer which columns were not found. The IDs will always be printed no matter what.

Alhough extremely similar, this approach to filter has its benifits. If you only wanted 1 column, instead of filtering out 5 options, you can instead filter in the one you want. Or in reverse, if you only wanted to drop one column you can simple type out one column name rather than 5.

### Ordering
Make-A-Date allows the user to print the output in a specified output for any column, ID included by using `--or COLUMN`. For string values the order is based alphabetically, where as ID and Date order is integer specified. 

The user can requeset the order to be **asc**ending or **desc**ending by following the `--or` flag with `ASC` or `DESC` after the chosen column, however if nothing is specified it automtically defaults to ascending.

``` bash
    ./NAME -fi idea --or idea ASC
```
[IMAGE-ORDERING-IDEA-BY-IDEA-ASC]

``` bash
    ./NAME -fi idea --or id desc
```
[IMAGE-ORDERING-IDEA-BY-ID-DESC]

``` bash
    ./NAME -fi idea --or id 
```
[IMAGE-ORDERING-IDEA-BY-DEFAULT]

#### Date Syntax
The Date format is a little differet. Due to the limitations of C and my own preference - it is stored and treated as a string value. This allows for Date plans that are seasnoal or an annual event to be stored using X as a place holder. For example a pumkin carving date during halloween can be repreesnted as "X/10/X".

This also comes with a drawback. As they are treated as string, when ordering by the "Date" column it is ordered alphanumerically. This means that the following dates "01/01/26", "02/01/26" and "01/02/26" would be ordered as "01/01...", "01/02/..." and "02/01..." due to the day being compared before the month or year. As search this required a unique solution.

[WIP] To solve this, the strings are broken down into Day, Month and Year using a function and compared in reverse order.This allows the user to sort the date data by date correctly.

"X" values are stored as a infinitly large number and depending wether or not they fall on the day, month or year they are treated differently. For example, anything with an X in the year is towards the end as it can be done every year.

### Has [WIP]
The `--has` option flag takes in a string value as an argument and allows the user to search the database, only returning strings that match the input. This allows the user to filter the search even more finely. 

For example, if a user wanted to return all dates that involve "coffee" or "happy hour", they can run the program with their chosen filters and order followed by `--has coffee` or `--happy hour`. This will limit what is printed to only include dates with those chosen words.

This input must be surrounded by `"`s and is case sensitive and [WIP????] must match exactly to what is found, so "has happy hour" and "happy hour" will not match.

```bash
    ./NAME --has "coffee"
```
[IMAGE-OF-HAS-COFFEE]

### In [WIP]

The option argument `--in` allows the user to specify in which column the search takes place. It accepts a single argument as a correct column name and will not run unless given a correct value. 

This limit in the search area not only allows for faster speeds when searching large data sets but also allows for more specific filter. For example, you may have saved a bar that also serves coffee and mention this in the notes, a specific cafe saved under the type "Coffee" and hypothetically a restaurant that has Coffee in the name. If you wanted to specifically return the restaurant then the `--in name` option allows for this by limiting the search for coffee in name.

The option must be followed by a correctly spelt column name and will only accept multiple column names to better customise the search criteria.

```bash
    ./Name --has "Coffee" --in name
```

```bash
    ./Name --has "Coffee" --in name notes
```

### Constants/Definitions
Due to the differing width of the users screen, and to further improve user experience and customisation, allows for different printing widths. Admitedly harder to customise, the user can adapt the C code to change how wide each column can be in character length.

At the top in constants the user will find the following:
```C
//Defining cloumn widths
    #define IDEA_WIDTH 15
    #define TYPE_WIDTH 15
    #define SEASONAL_WIDTH 10
    #define DATE_WIDTH 7
    #define LOCATION_WIDTH 10
    #define NOTES_WIDTH 30
```

These values are hard coded constants that allow the character width limits to be changed for each adn every column. For example, if the user didnt want to occupy the width of their screen with notes due to techincal limitations or preference, they can reduce the notes width, and where the notes would have overflown the program adds a "..." to signify it is wider than it is allowed to be.
