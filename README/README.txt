These applications in concept were made to help me keep track of date ideas. The concept was to keep me from getting rusty, that is why i chose to do it in C with an open scope that will allow me to
develope my web dev skills and then mobile development skills. 

If you are reading this please keep in mind that this is still a work in project and is little more than a JSON parser with formatting capaiblities but hopefully soon it will be more. As of right now enjoy
the text based console proof of concept.

To begin, here is a simple guide to show how best to use it.

[1] Place the MAD-V1 (Make a Date Version 1) in the same directory as a JSON file with date inside in the format of:
    [
    {
        "id": [INT],
        "Idea": "[TEXT]",
        "Type": "[TEXT]",
        "Seasonal": "[TEXT]",
        "Date": "X/X/X",
        "Location": "[TEXT]",
        "Notes": "[TEXT]" 
    },
    {...},
    ]

    Please keep in mind the date attribute is text based and the Xs can be replaced by actual numbers or left as Xs in general for example halloween every year would be written as 31/10/X

    If there is no file the application will make one.

[2] Please compile the C file with the following command:
        gcc MAD-V1.c -o MAD-V1 -lcjson
    as I have used an external library to read JSON, please feel free to name the compiled file what ever you would like but I will continue to use MAD-V1 in the rest of this document 

[3] To use the application you can run the compiled C file in various ways.
    The first argument will be specifiying if the program is run to read the JSON file or to write to it. This will be shown by typing either (it is NOT case sensitive)
        [3.1] ./MAD-V1 read
                Which will print out everything found 
            [3.1.1] The following argument will specify wether or not to filter columns into the print or out of the print (but will not accept both)
                [3.1.1.1] "./MAD-V1 read --fi [col1] [col2]" will only print out (Filter In) columns 1 and 2 and nothing else in the order are specified in the JSON file
                [3.1.1.2] "./MAD-V1 read --fo [col1] [col2]" will print out eveyrthing but (Filter Out) columns 1 and 2 in the order they are specified in the JSON file
        
                These argument can be used along side the ordering argument

            [3.1.2] The following argument will specify how to order the print by the column and then by ASCending or DESCending
                [3.1.2.1] "./MAD-V1 read --fX [col1] [col2] ... --or [col3] asc" will print the columns filtered by column 3 with the values ascending as they are printed
                [3.1.2.2] "./MAD-V1 read --fX [col1] [col2] ... --or [col3] desc" will print the columns filtered by column 3 with the values descending as they are printed
                
                This argument can be used along side the filtering argument. 
                Additionally, the ASC/DESCending values will effect the integers as expected and will print text values alphabetically with "a" being treated as the "smallest" value and "z" as the largest

                (NOT IMPLEMENTED YET)

            [3.1.3] The following argument will only print dates where the value is found insidef the date 
                [3.1.3.1] "./MAD-V1 read --has [VALUE]" (NOT IMPLEMENTED YET)
            and can be used along side the following argument to specify that the value MUST be found inside a specific column 
                [3.1.3.2] "./MAD-V1 read --has [VALUE] --in [COL]" (NOT IMPLEMENTED YET)
    OR

        [3.2] ./MAD-V1 write
            This will propmt the user to enter the date details into the console and will add the data to the JSON file

