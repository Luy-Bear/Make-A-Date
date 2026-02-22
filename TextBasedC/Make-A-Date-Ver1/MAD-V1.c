    #include <stdio.h> //prints
    #include <string.h> //string copmarisons
    #include <ctype.h> //strupr
    #include <cjson/cJSON.h> //JSON handling
    #include <stdlib.h> //Malloc stuff
    #include <unistd.h> //flag options
    #include <getopt.h>
    // Explicitly declare getopt variables
    extern char *optarg;
    extern int optind, opterr, optopt;

    //Defining cloumn widths
    #define IDEA_WIDTH 15
    #define TYPE_WIDTH 15
    #define SEASONAL_WIDTH 10
    #define DATE_WIDTH 8
    #define LOCATION_WIDTH 10
    #define NOTES_WIDTH 30

    //Defining Col Values
    #define ID 0
    #define IDEA 1
    #define TYPE 2
    #define SEASONAL 3
    #define DATE 4
    #define LOCATION 5
    #define NOTES 6

    int DateHas(char* HasVal, int* InColFlagArr, cJSON*  Date);

    //Uses quick sort to sort dates into order
    void QuickSortDates(int Lower, int Upper, cJSON* Dates[], int ValCol);

    //Function that compares value between JSON dates
    int CompareJSONDateVals(cJSON* a, cJSON* b, int ValCol);

    // Uppercase a string
    char * StrToUpper(char *in);

    //Returns a string in memory 
    char * ReturnStrXLength(int X, char* str);
    
    //Returns JSON attributes but makes it more readable
    int ReturnJsonID(cJSON * JSON);
    char * ReturnJsonIdea(cJSON * JSON);
    char * ReturnJsonType(cJSON * JSON);
    char * ReturnJsonSeasonal(cJSON * JSON);
    char * ReturnJsonDate(cJSON * JSON);
    char * ReturnJsonLocation(cJSON * JSON);
    char * ReturnJsonNotes(cJSON * JSON);
    

    //print JSON function
    void PrintJSONObjs(int FilterFlagArr[], cJSON *Dates[], int DateNums, char *order_direction);

    //Custom function to compare Date's Date for ordering
    int DateCmp(char* DateA, char* DateB);
    //Custom function to deal with generic date dates, so it handles -- as if XX
    int CompareDateComponent(const char* a, const char* b);

    //Write Section
    //Removes \n on fgets fn
    void RemoveNewline(char* str);
    //Checks for user input
    char* IdeaValid(char* UserIdea);


    int main(int argc, char *argv[]){
        //Argc argument counter, arv is argument vector, array of char pointers listing all args
       printf("\n");
        //If there are command line arguments - run, else quit
        if(argc > 1){
            // Check if read or write
            if(strcmp(StrToUpper(argv[1]), "READ") == 0){
                // FLAG PARSING SECTION
                int opt;
                char filter_field = '\0';
                char *filter_fields[6]; //Max nuber is 6 (number of cols in json) 
                int filter_count = 0;
                char *order_field = NULL;
                int order_field_int = -1;
                char *order_direction = NULL;
                char *HasVal = NULL;
                optind =2; //Option index

                // Define long options: Can change first arg to be command line, lat arguemnt is the case select option
                struct option long_options[] = {
                    {"fi", required_argument, 0, 'i'},
                    {"fo", required_argument, 0, 'o'},
                    {"or", required_argument, 0, 'r'},
                    {"has", required_argument, 0, 'h'},
                    {"in", required_argument, 0, 'n'},
                    {0, 0, 0, 0}  // Terminator
                };

                // Flag to check f user called both filter options at once
                int DoulbeFilterFlag = 0;

                //--Has and --In Vars
                int InColFlagArr[6] ={0,0,0,0,0,0}; //Makes 6 flags for all columns - ID not searchable, also initialise at 0 to remove garbage data.

                // Parse filter arguments and options
                // opt will check letter by letter for --fi or --fo and/or --or and/or has and --or --in unkown option found (e.g -z) defualt is hit, if nothing else to check -1 returned 
                while ((opt = getopt_long(argc, argv, "", long_options, NULL)) != -1) {
                    switch (opt) {
                        //If it finds fi it will...
                        case 'i':
                            if(!DoulbeFilterFlag){
                                DoulbeFilterFlag=1;
                                filter_field = 'i';
                            }
                            else{
                                fprintf(stderr, "Usage: Use of conflicting filter options (both Filter In and Filter Out detected)");
                                exit(EXIT_FAILURE);
                            }
                            // Adds argument into fields array
                            filter_fields[filter_count++] = StrToUpper(optarg);
                            // Continue collecting non-option arguments - 
                            // if there are more arguments than the one i am looking at and the next argument doesnt start wit a -f we keep going.
                            while (optind < argc && argv[optind][0] != '-') {
                                filter_fields[filter_count++] = StrToUpper(argv[optind]);
                                optind++;
                            }
                            break;                        //If it finds fo it will...
                        case 'o':
                            if(!DoulbeFilterFlag){
                                DoulbeFilterFlag=1;
                                filter_field = 'o';
                            }
                            else{
                                fprintf(stderr, "Usage: Conflicting filter options (both Filter In and Filter Out detected)\n");
                                exit(EXIT_FAILURE);
                            }
                            // Adds argument into fields array
                            filter_fields[filter_count++] = StrToUpper(optarg);
                            // Continue collecting non-option arguments - 
                            // if there are more arguments than the one i am looking at and the neet argument doesnt start wit a -f we keep going.
                            while (optind < argc && argv[optind][0] != '-') {
                                filter_fields[filter_count++] = StrToUpper(argv[optind]);
                                optind++;
                            }
                            break;
                        // if it finds o it will...
                        case 'r':
                            // Take the argument as what to order it by 
                            order_field = StrToUpper(optarg);

                            //Check what col to order by to pass into helper function to compare values of dates
                            if(strcmp(order_field, "ID") == 0) order_field_int = ID;
                            else if(strcmp(order_field, "IDEA") == 0) order_field_int = IDEA;
                            else if(strcmp(order_field, "TYPE") == 0) order_field_int = TYPE;
                            else if(strcmp(order_field, "SEASONAL") == 0) order_field_int = SEASONAL;
                            else if(strcmp(order_field, "DATE") == 0) order_field_int = DATE;
                            else if(strcmp(order_field, "LOCATION") == 0) order_field_int = LOCATION;
                            else if(strcmp(order_field, "NOTES") == 0) order_field_int = NOTES;
                            else {
                                // Invalid column name, exit program
                                fprintf(stderr, "Error: '%s' is not a valid column name for ordering\n", order_field);
                                fprintf(stderr, "Valid columns: ID, IDEA, TYPE, SEASONAL, DATE, LOCATION, NOTES\n");
                                exit(EXIT_FAILURE);
                            }

                            // if there are more options, and theyre not a flag
                            if (optind < argc && argv[optind][0] != '-') {// take argumnet as direction but only if ASC or DESC (works for EVERYTHING)
                                // strcmp returns 0 if string are identical, <0 if str1 comes before alphabetically and >0 if str1 comes after str 2 alphabetically 
                                if(strcmp((StrToUpper(argv[optind])), "ASC") == 0 || strcmp((argv[optind]), "DESC") == 0){
                                    order_direction = StrToUpper(argv[optind]);
                                    optind++;
                                }
                                else{
                                    //If the order is not ascending or descending it is not valid
                                    fprintf(stderr, "Not a valid ordering direction - must be ASCending or DESCending\n");
                                    exit(EXIT_FAILURE);
                                }
                            } 
                            printf("\n");
                            break;
                        
                        case 'h':
                            //When user has --has argument flag it will accept the first value after the option INBETWEEN SPEECH MARCHS
                                //i.e: '"word1 word2 ..... wordx"' but not 'word1 word2 ... wordx'
                            HasVal = optarg;
                            // Check for extra arguments (error case)
                            if (optind < argc && argv[optind][0] != '-') {
                                fprintf(stderr, "Error: Unexpected argument '%s' after --has\n", argv[optind]);
                                fprintf(stderr, "Did you forget quotes? Use: --has \"word1 word2 ... wordx\"\n");
                                exit(EXIT_FAILURE);
                            }

                            break;

                        case 'n':{ //These is if user puts "in" in the arguemnts
                            // Valid column names
                            const char* valid_cols[] = {"IDEA", "TYPE", "SEASONAL", "DATE", "LOCATION", "NOTES"};
                            
                            // Process first column (from optarg)
                            char* col = StrToUpper(optarg);
                            for(int i = 0; i < 6; i++) {
                                if(strcmp(col, valid_cols[i]) == 0) {
                                    InColFlagArr[i] = 1;
                                    break;
                                }
                                if(i==5){printf("Invalid column name for 'in' argument: '%s'\n", optarg);}
                            }
                            
                            // Process additional columns
                            while(optind < argc && argv[optind][0] != '-') {
                                col = StrToUpper(argv[optind]);
                                for(int i = 0; i < 6; i++) {
                                    if(strcmp(col, valid_cols[i]) == 0) {
                                        InColFlagArr[i] = 1;
                                        break;
                                    }
                                    if(i==5){printf("Invalid column namefor 'in' argument: : '%s'\n", argv[optind]);}
                                }
                                optind++;
                            }
                            printf("\n");
                            break;
                        }
                        default:
                            // if another flag found its wrong - return error message and exit
                            fprintf(stderr, "Usage: %s -f(i|o) field1 [field2...] -Or field [asc|desc]\n", 
                                    argv[0]);
                            exit(EXIT_FAILURE);
                    }
                }

                //Once all arguemnts parsed....
                //Print what searching for
                printf("Searching for %s", HasVal);
                printf("\n");
                //check if no --in columns specified BUT ALSO --has value specified - in that case, print eveything 
                if((InColFlagArr[0] + InColFlagArr[1] + InColFlagArr[2] + InColFlagArr[3] + InColFlagArr[4] + InColFlagArr[5]) == 0 && HasVal != NULL){
                    for(int i = 0; i < 6; i++) {
                        InColFlagArr[i] = 1;
                    }
                    printf("No valid columns to search in found, defaulting to search all columns");
                    printf("\n");
                }
                //Checks if valid has and in arguments provided
                if(HasVal != NULL){
                    if((InColFlagArr[0] + InColFlagArr[1] + InColFlagArr[2] + InColFlagArr[3] + InColFlagArr[4] + InColFlagArr[5]) != 0){
                        //Else tell the user what cols are valid and are going to be searched
                        printf("The following columns will be searched: ");
    
                        const char* display_names[] = {"Idea", "Type", "Seasonal", "Date", "Location", "Notes"};
    
                        int first = 1;
                        for(int i = 0; i < 6; i++) {
                            if(InColFlagArr[i]) {
                                if(!first) printf(", ");
                                printf("%s", display_names[i]);
                                first = 0;
                            }
                        }
                        printf("\n");
                    }
                }
                else{
                    if((InColFlagArr[0] + InColFlagArr[1] + InColFlagArr[2] + InColFlagArr[3] + InColFlagArr[4] + InColFlagArr[5]) != 0 ){printf("No valid In value given to search for, showing all results...\n");}
                }
                printf("\n");

                // Filter Flag Logic - Takes user filters and sets up flags for future use
                // [0    1    2        3    4        5    ]
                // [IDEA TYPE SEASONAL DATE LOCATION NOTES]

                int FilterFlagArr[6]={0}; //Makes 6 flags for all columns - ID not filterable, also initialise at 0 to remove garbage data.

                //If not filter specified print eveyrthing so set flag values to 1
                if(filter_field == '\0'){
                    //Set flag values to 1
                    for (int i = 0; i < 6; i++) {
                        FilterFlagArr[i] = 1;
                    }
                }
                else{
                
                    //Loop through all filter fields and uses an if else str compare ladder to set column flags
                    for(int i=0; i< filter_count; i++){
                        //Uses more mem short term but speeds up copmuting by uppercasing once rather than several times
                        char *upper = StrToUpper(filter_fields[i]);

                        //If the uppercase string == column X, set flag X to 1. Array index adjusted as not doing IDs but ordering i will so need 0 to be id therefore offsetting vals in this case
                        if(strcmp(upper, "IDEA") == 0) {
                            FilterFlagArr[IDEA-1] = 1;
                        }
                        else if(strcmp(upper, "TYPE") == 0) {
                            FilterFlagArr[TYPE-1] = 1;
                        }
                        else if(strcmp(upper, "SEASONAL") == 0) {
                            FilterFlagArr[SEASONAL-1] = 1;
                        }
                        else if(strcmp(upper, "DATE") == 0) {
                            FilterFlagArr[DATE-1] = 1;
                        }
                        else if(strcmp(upper, "LOCATION") == 0) {
                            FilterFlagArr[LOCATION-1] = 1;
                        }
                        else if(strcmp(upper, "NOTES") == 0) {
                            FilterFlagArr[NOTES-1] = 1;
                        }
                        else{
                            printf("User input of '%s' not found as column name for fields to filter in/out\n", filter_fields[i]);
                        }
                    }
                    printf("\n");
                }
                
                
                //If user species to filter out, flip flags
                if(filter_field=='o'){
                    for (int i = 0; i < 6; i++) {
                        FilterFlagArr[i] = !(FilterFlagArr[i]);
                    }
                }

                if((FilterFlagArr[0] + FilterFlagArr[1] + FilterFlagArr[2] + FilterFlagArr[3] + FilterFlagArr[4] + FilterFlagArr[5])==0){
                    printf("No valid columns can be printed, please input valid arguments for the filter command\n");
                    return 1;
                }
                
                
                if(
                    filter_field == 'i'){printf("Filter In fields: ");
                    
                    //Display filter arguments
                    if(FilterFlagArr[0]){printf("Idea ");}
                    if(FilterFlagArr[1]){printf("Type ");}
                    if(FilterFlagArr[2]){printf("Seasonal ");}
                    if(FilterFlagArr[3]){printf("Date ");}
                    if(FilterFlagArr[4]){printf("Location ");}
                    if(FilterFlagArr[5]){printf("Notes ");}
                    printf("\n");
                    
                }
                else if(
                    filter_field == 'o'){printf("Filter Out fields: ");

                    //Display filter arguments
                    if(FilterFlagArr[0]){printf("Idea ");}
                    if(FilterFlagArr[1]){printf("Type ");}
                    if(FilterFlagArr[2]){printf("Seasonal ");}
                    if(FilterFlagArr[3]){printf("Date ");}
                    if(FilterFlagArr[4]){printf("Location ");}
                    if(FilterFlagArr[5]){printf("Notes ");}
                    printf("\n");   
                }

                //Display Order direction
                if (order_field) {
                    printf("Order by: %s %s\n", order_field, 
                        order_direction ? order_direction : "ASC (by default)");
                }
 
                
                
 
                // Open file to read
                FILE * DatesFile = fopen("Dates.json", "r");

                //Check to see if file exists
                if(DatesFile == NULL){
                    printf("Error: Unable to open the file.\n");
                    return 1;
                }

            //Making buffer the file size exactly
                //Get filesize
                fseek(DatesFile, 0, SEEK_END);
                long file_size = ftell(DatesFile);
                fseek(DatesFile, 0, SEEK_SET);

                // Allocate mem for buffer
                char * buffer = malloc(file_size +1); //+1 for null point terminator
                if (buffer == NULL) {
                    //if buffer is not a valid number then throw error adn close data file.
                    printf("Error: Memory allocation failed.\n");
                    fclose(DatesFile);
                    //If the files not vaild no point continuining program so exit.
                    return 1;
                }

                // Read file into buffer now that we know its valid
                fread(buffer, 1, file_size, DatesFile);
                buffer[file_size] = '\0';  // Add null terminator
                //Once data in buffer close data file.
                fclose(DatesFile);

                // Pase JSON
                //Now the file is closed and inside the buffer we can parse JSON
                cJSON *json = cJSON_Parse(buffer);
                free(buffer);  
                
                // Check to see if theres any valid JSON to look through
                if (json == NULL) {
                    const char *error_ptr = cJSON_GetErrorPtr();
                    if (error_ptr != NULL) {
                        printf("Error: %s\n", error_ptr);
                    }
                    exit(EXIT_FAILURE);
                }

                // Check if it's an array (i.e more than one JSON objects (more than one date [{date1...},{date2...}] ))
                if (!cJSON_IsArray(json)) {
                    printf("Error: Expected JSON array\n");
                    cJSON_Delete(json);
                    return 1;
                }

                // Get array size for loop
                int array_size = cJSON_GetArraySize(json);
                
                printf("\nOut of %d dates, printing ", array_size); //PRINT CONTINUES AFTER THE ARRAY LIMITED

                

                // Have an empty array to populate with dates to print
                // We are using this as "json" is a linked list struct not a c array, Dates is an array that points to the original JSON array which lets you sort them with traditional sorting algos but moving the pointers leaving original structure unchanged
                    // This means i dont accidentally corrupt or change original structure and for array sorts it allows jumping to indicies not o(n) linear look ups by chekcing next n times
                    // Overall pointer array uses little mem, O(1) access time and simpler to implement without running risk of messing up json struct - at the price of a little more mem
                cJSON **Dates = calloc(array_size, sizeof(cJSON *));
                

                //Clean up free incase no mem to allocated
                if (Dates == NULL) {
                    fprintf(stderr, "Error: Memory allocation failed for Dates array\n");
                    cJSON_Delete(json);
                    exit(EXIT_FAILURE);
                }

                int actual_count = 0;  // Compact array counter (no gaps)

                //Adds all the JSON elements into Dates array
                for(int i = 0; i < cJSON_GetArraySize(json); i++) {
                    // printf(".\n");
                    cJSON *Date = cJSON_GetArrayItem(json, i);
                    //If there is a value to search....
                    if(HasVal != NULL){
                        //Flags are either all set to 1 or select few, pass flag array into helper function
                        if(DateHas(HasVal, InColFlagArr, Date)){ 
                            // if found, add to dates using actual_count (creates compact array with no gaps)
                            Dates[actual_count] = Date;
                            actual_count++;
                        }
                        //else skip and do nothing, so Dates only has Dates with correct search value in them
                        // printf("bah\n");
                    }
                    else{
                        Dates[actual_count] = Date;
                        actual_count++;
                    }
                }

                // Update array_size to reflect actual number of dates added (not total JSON items)
                array_size = actual_count;                

                //CONTINUTATION OF PRINT STATEMENT "Out of %d dates, printing "
                printf("%d...\n", array_size);
                printf("____________________________________________________________________________\n\n\n\n");


                // If user specifies a col to order by, order it in place in Dates arr and then print it using quick sort ...
                if(order_field != NULL){        
                    //Implemented quick sort fn as a recursive function - order_field_int is flag set in the argument switch case for options, specifies what field to order by
                    QuickSortDates(0, array_size-1, Dates, order_field_int); 
                }
                // ....if no order specified skip the if statement above and just print out in order it is read in
                PrintJSONObjs(FilterFlagArr, Dates, array_size, order_direction);
                free(Dates);
                cJSON_Delete(json);

                exit(EXIT_SUCCESS);
            }
            else if (strcmp(StrToUpper(argv[1]), "WRITE") == 0){
                printf("GOT INTO WRITE\n");
                //TODO: implement this!

                //Check if all arguments specified (6 args + write command + run command):
                if(argc == 8){
                    //TODO: FINISH THIS

                    // Extract args 
                    char* UsrIdea = argv[2];
                    char* UsrType = argv[3];
                    char* UsrSeasonal = argv[4];
                    char* UsrDate = argv[5];
                    char* UsrLocation = argv[6];
                    char* UsrNotes = argv[7];
                

                    //Add vars to JSON
                    printf("%s %s %s %s %s %s", UsrIdea,UsrType,UsrSeasonal,UsrDate,UsrLocation,UsrNotes);

                    // Check if correct type (Reuse function for the empty command run)
                    // Add to vars
                    
                }
                //Check if just write command specified (1 arg + write + run command)
                else if(argc == 2){
                    //Begin process of asking user for one input at a time, check type and validity using helper function x6
                    
                    // Fixed-size buffers NEEDED for user input
                    char UsrIdea[100];
                    char UsrType[50];
                    char UsrSeasonal[50];
                    char UsrDate[9];      // Exactly 8 chars + null terminator
                    char UsrLocation[100];
                    char UsrNotes[200];

                    //Ask for Idea
                    printf("Enter the date Idea: ");
                    //Scan line, stops at '\n' between the "
                    fgets(UsrIdea, sizeof(UsrIdea), stdin);
                    //Remove \n from var
                    RemoveNewline(UsrIdea);
                    //Function to verify validity
                    //TODO CHECK VALIDITY

                    //Ask for Type
                    printf("Enter the date type: ");
                    //Scan line, stops at '\n' between the "
                    fgets(UsrType, sizeof(UsrType), stdin);
                    //Remove \n from var
                    RemoveNewline(UsrType);
                    //Function to verify validity
                    //TODO CHECK VALIDITY

                    //Ask for Seasonal
                    printf("Enter what seasonal the date is for or \"NONE\" if not applicable: ");
                    //Scan line, stops at '\n' between the "
                    fgets(UsrSeasonal, sizeof(UsrSeasonal), stdin);
                    //Remove \n from var
                    RemoveNewline(UsrSeasonal);
                    //Function to verify validity
                    //TODO CHECK VALIDITY

                    //Ask for Date
                    printf("Enter the specific date in the form dd/mm/yy OR use \"--\" if theres no specific day/month/year for it: ");
                    //Scan line, stops at '\n' between the "
                    fgets(UsrDate, sizeof(UsrDate), stdin);
                    //Remove \n from var
                    RemoveNewline(UsrDate);
                    //Function to verify validity
                    //TODO CHECK VALIDITY

                    //Ask for Date
                    printf("Enter the dates location: ");
                    //Scan line, stops at '\n' between the "
                    fgets(UsrLocation, sizeof(UsrLocation), stdin);
                    //Remove \n from var
                    RemoveNewline(UsrLocation);
                    //Function to verify validity
                    //TODO CHECK VALIDITY

                    //Ask for Note
                    printf("Enter the any additional notes for the date: ");
                    //Scan line, stops at '\n' between the "
                    fgets(UsrNotes, sizeof(UsrNotes), stdin);
                    //Remove \n from var
                    RemoveNewline(UsrNotes);
                    //Function to verify validity
                    //TODO CHECK VALIDITY

                    //Add vars to JSON
                    printf("%s %s %s %s %s %s", UsrIdea,UsrType,UsrSeasonal,UsrDate,UsrLocation,UsrNotes);


                }
                else{
                    //Error message saying to either write all args or just write.
                    printf("ERROR: Incorect number of arguments following \"Write\" command\nPlease use either the write command and input your arguments when asked to or include all the 6 arguments at once in the following order: Idea, Type, Seasonal, Date, Locaton and Notes\n\n" );
                    printf("WARNING: Any multiple word inputs must be surrounded by \"s so program treats it as a single input\n\n");
                    exit(EXIT_FAILURE);
                }
                
                exit(EXIT_SUCCESS);
            }
            else{
                printf("%s is not a valid operation, try read or write.\n\n", argv[1]);
                exit(EXIT_FAILURE);
            }
            
            printf("USAGE: Make-A-Date required more arguemnts. Please run specifying WRITE or READ and optionally for read filter in or filter out options (--fi or --fo), order by a column and specifically by ASCending or DESC and even select entries WHERE values are found\n");
            exit(EXIT_FAILURE);
        }


        // Fail case, no args inputed,
        printf("No command line arguments inputed, specify if it is a read or a write \ninteraction followed if necessary by the order amd filter requierments\n\n");
        return 1;
    }


    void QuickSortDates(int LowerLim, int UpperLim, cJSON* Dates[], int ValCol){
        // Check if i can do median of 3...
        //if one do nothing, leave as cant sort anymore. 
        if(!(LowerLim >= UpperLim)){
            //if two sort and return
            if(UpperLim-LowerLim == 1){
                //If lower lim date is bigger than upper lim (a>b), swap using temp value, else its in order and do nothing
                if(CompareJSONDateVals(Dates[LowerLim],Dates[UpperLim],ValCol)>0){
                    cJSON* temp = Dates[UpperLim];
                    Dates[UpperLim] = Dates[LowerLim];
                    Dates[LowerLim] = temp; 
                }
            }
            //otherwise theres more than 2 elements and you can do median logic!
            else{
            int PivotIndex; //Var for pivot index to be stored in once returned.
            
            //Median of Three Approach: Check 3 values find median and use that value as index for pivot
                //Calculate the median value out of indecies 0, N/2, N - 6 Possible condiitons: a b c || a c b || c b a || b c a || c a b || b a c	
                // Checks conditions where a is smaller than b, then check if b smaller than c to determin if abc or acb
                //where a = LowerLim, b = (LowerLim+UpperLim)/2 , c = UpperLim
                
                if(CompareJSONDateVals(Dates[LowerLim], Dates[(LowerLim+UpperLim)/2], ValCol) < 0){ //a < b but where is c?
                    if(CompareJSONDateVals(Dates[(LowerLim+UpperLim)/2], Dates[UpperLim], ValCol) < 0){PivotIndex = (LowerLim+UpperLim)/2;}//a b c
                    else if(CompareJSONDateVals(Dates[UpperLim], Dates[LowerLim], ValCol) < 0){PivotIndex = LowerLim;}//c a b
                    else{PivotIndex = UpperLim;}//a c b
                        
                }
                else{//b < a but where is c?
                    if(CompareJSONDateVals(Dates[LowerLim], Dates[UpperLim], ValCol) < 0){PivotIndex = LowerLim;}//b a c
                    else if(CompareJSONDateVals(Dates[(LowerLim+UpperLim)/2], Dates[UpperLim], ValCol) < 0){PivotIndex = UpperLim;} //b c a
                    else{PivotIndex = (LowerLim+UpperLim)/2;}//c b a
                }

                //Now have pivot @ index stored with PivotIndex
                int LPointer = LowerLim, RPointer = UpperLim-1; //Rpointer is -1 from limit to ignore pivot

                //check if pivot is last elem, if not swap them
                if(PivotIndex != UpperLim){
                    cJSON* temp = Dates[PivotIndex];
                    Dates[PivotIndex] = Dates[UpperLim];
                    Dates[UpperLim] = temp;
                    PivotIndex = UpperLim; //AFTER MOVING PIVOT TO FAR RIGHT, ADJUST PIVOT INDEX TO REFLECT THIS!!
                }


                //Begin to order the Dates with pointers passed....
                while(LPointer <= RPointer){
                    if(CompareJSONDateVals(Dates[LPointer], Dates[PivotIndex], ValCol) > 0){ //Is LPointer > Pivot Val (needs changing)
                        if(CompareJSONDateVals(Dates[RPointer], Dates[PivotIndex], ValCol) < 0){ //Is Right value < pointer value (needs changing)
                            //Swap the pair round
                            cJSON* temp = Dates[RPointer];
                            Dates[RPointer] = Dates[LPointer];
                            Dates[LPointer] = temp;

                            //Shift pointers to next value
                            RPointer--;
                            LPointer++;
                        }
                        else{ //Just move RPointer and wait for the RPointer to find a misfit to swap
                            RPointer--;
                        }
                    }
                    else{
                        LPointer++; //Shift pointer along by one - doestn need changing so dont need to keep looking at val
                        //if R pointer val > pivot val 
                        if(CompareJSONDateVals(Dates[RPointer], Dates[PivotIndex], ValCol) > 0) //Value Doesnt need changing, so move pointer
                            {//shift R pointer by -1
                                RPointer--;
                            }   
                        // (else needs to swap but cant becuase L doesnt so keep pointer the same)
                    }
                
                }
                //Ordering done, now tneed to place pivot in place.
                //LPointer points at value that is bigger than the pivot so swap this with pivot which is at the upper index.    
                cJSON* temp = Dates[UpperLim];
                Dates[UpperLim] = Dates[LPointer]; //Putes LPointer val on right side
                Dates[LPointer] = temp;            //Puts Pivot in place
                //Recursive call on remaining sectinos of list
                QuickSortDates(LowerLim, LPointer-1, Dates, ValCol); //LPointer, just swaped the last "bigger than pivot" with the far right elem (pivot), so Lpointer points at pivot. So do one less than pivot
                QuickSortDates(LPointer+1, UpperLim, Dates, ValCol); //As LPointer points at pivot, use the next obj in array as the lower lim.
                
            
                



            }
        }
        return;        
    }

    //Copmares the values specified by ValCol of two dates and return which one is bigger than or equal to or smaller than the other as:
    //return x < 0
    //return x = 0
    //return x > 1 
    int CompareJSONDateVals(cJSON* a, cJSON* b, int ValCol){
        switch(ValCol) {
            case ID:  // Integer comparison
                return ReturnJsonID(a) - ReturnJsonID(b);
                
            case IDEA:  // String comparison
                return strcmp(ReturnJsonIdea(a), ReturnJsonIdea(b));
                
            case TYPE:
                return strcmp(ReturnJsonType(a), ReturnJsonType(b));
                
            case SEASONAL:
                return strcmp(ReturnJsonSeasonal(a), ReturnJsonSeasonal(b));
            
            case DATE:
                //Special Case: Allows for ordering of dates by "year month day", when stored as "day month year"
                return DateCmp(ReturnJsonDate(a), ReturnJsonDate(b));
            case LOCATION:
                return strcmp(ReturnJsonLocation(a), ReturnJsonLocation(b));
        
            case NOTES:
                return strcmp(ReturnJsonNotes(a), ReturnJsonNotes(b));
    
            default:
                return 0;  // Should never happen
        }
    }

    char * StrToUpper(char *in){
        int i = 0;
        while(in[i] != '\0'){
            in[i] = toupper(in[i]);
            i++;
        }
        return in;
    }

    // CALL FREE ON RETURNED STRING (ALSO X is ... inclusive of str length)
    char * ReturnStrXLength(int X, char* str){
        // DOESNT modify in place

        // Edge case: X too small for "..."
        if(X < 4){return NULL;}

        int inStrLen = strlen(str);
        char *result = malloc(X + 1);  // +1 for null terminator

        // If string is perfect size return it as is
        if(inStrLen == X){
            strcpy(result, str);
            return result; //doing it this way so its always freed no matter what so no mem leaks occur
        }

        //If its too big, add "..."
        if(inStrLen > X){
            strncpy(result, str, X-3);
            result[X-3] = '.';
            result[X-2] = '.';
            result[X-1] = '.';
            result[X] = '\0'; 
        }
        else if (inStrLen < X) //if its too small, add padding
        {
            strcpy(result, str);
            int i = inStrLen;  // Start where string ends
            while(i < X){
                result[i] = ' ';
                i++;
            }
            result[X] = '\0';
        }
        return result;
    }


    int ReturnJsonID(cJSON * JSON){
        return(cJSON_GetObjectItemCaseSensitive(JSON, "id")->valueint);
    }

    char * ReturnJsonIdea(cJSON * JSON){
        return(cJSON_GetObjectItemCaseSensitive(JSON, "Idea")->valuestring);
    }

    char * ReturnJsonType(cJSON * JSON){
        return(cJSON_GetObjectItemCaseSensitive(JSON, "Type")->valuestring);
    }

    char * ReturnJsonSeasonal(cJSON * JSON){
        return(cJSON_GetObjectItemCaseSensitive(JSON, "Seasonal")->valuestring);
    }

    char * ReturnJsonDate(cJSON * JSON){
        return(cJSON_GetObjectItemCaseSensitive(JSON, "Date")->valuestring);
    }

    char * ReturnJsonLocation(cJSON * JSON){
        return(cJSON_GetObjectItemCaseSensitive(JSON, "Location")->valuestring);
    }

    char * ReturnJsonNotes(cJSON * JSON){
        return(cJSON_GetObjectItemCaseSensitive(JSON, "Notes")->valuestring);
    }

    void PrintJSONObjs(int FilterFlagArr[], cJSON *Dates[], int DateNums, char* order_direction){
        // Set default direction if not specified
        if(order_direction == NULL) {
            order_direction = "ASC";
        }

        int Descending = (strcmp(order_direction, "DESC") == 0); //if order direction is DESC, strcmp returns 0, Descending becomes true (1) 
        int i = Descending ? (DateNums-1) : 0; //If descending is true i is max index of dates to count in reverse, else it is 0 ready to count up 

        // PRINT COLUMN HEADERS
        
        //this is for dynamic separator printing
        int columns_printed = 0;
        int separator_length = 10;  // "Idea XXX: " is 10 chars wide
        
        printf("Idea ID | ");
        
        if(FilterFlagArr[0]) {
            char *formatted = ReturnStrXLength(IDEA_WIDTH, "IDEA");
            printf("%s | ", formatted);
            free(formatted);
            columns_printed++;
            separator_length += IDEA_WIDTH;
        }
        
        if(FilterFlagArr[1]) {
            char *formatted = ReturnStrXLength(TYPE_WIDTH, "TYPE");
            printf("%s | ", formatted);
            free(formatted);
            columns_printed++;
            separator_length += TYPE_WIDTH;
        }
        
        if(FilterFlagArr[2]) {
            char *formatted = ReturnStrXLength(SEASONAL_WIDTH, "SEASONAL");
            printf("%s | ", formatted);
            free(formatted);
            columns_printed++;
            separator_length += SEASONAL_WIDTH;
        }
        
        if(FilterFlagArr[3]) {
            char *formatted = ReturnStrXLength(DATE_WIDTH, "DATE");
            printf("%s | ", formatted);
            free(formatted);
            columns_printed++;
            separator_length += DATE_WIDTH;
        }
        
        if(FilterFlagArr[4]) {
            char *formatted = ReturnStrXLength(LOCATION_WIDTH, "LOCATION");
            printf("%s | ", formatted);
            free(formatted);
            columns_printed++;
            separator_length += LOCATION_WIDTH;
        }
        
        if(FilterFlagArr[5]) {
            char *formatted = ReturnStrXLength(NOTES_WIDTH, "NOTES");
            printf("%s", formatted);  // No " | " at the end
            free(formatted);
            columns_printed++;
            separator_length += NOTES_WIDTH;
        }

        printf("\n");
        // Print separator line dynamically

        if(columns_printed > 0) {
            separator_length += (columns_printed - 1) * 3;  // " | " separators
        }

        for(int j = 0; j < separator_length; j++) {
            printf("─");
        }
        printf("\n");



        //if decending true, check if i >= 0, otherwise check if it is max index or below)
        while(Descending ?(i>= 0) : (i<DateNums)){
            // printf("i: %d, Datenums: %d\n", i, DateNums);
            cJSON *Date = Dates[i];
            if (Date != NULL) {
                // Always print the FORMATTED ID
                printf("Idea %3d: ", ReturnJsonID(Date));
                
                // [0    1    2        3    4        5    ]
                // [IDEA TYPE SEASONAL DATE LOCATION NOTES]
                //Checks flags to print text
                
                if(FilterFlagArr[0]) {
                    char *formatted = ReturnStrXLength(IDEA_WIDTH, ReturnJsonIdea(Date));
                    printf("%s | ", formatted);
                    free(formatted);
                }
                
                if(FilterFlagArr[1]) {
                    char *formatted = ReturnStrXLength(TYPE_WIDTH, ReturnJsonType(Date));
                    printf("%s | ", formatted);
                    free(formatted);
                }
                
                if(FilterFlagArr[2]) {
                    char *formatted = ReturnStrXLength(SEASONAL_WIDTH, ReturnJsonSeasonal(Date));
                    printf("%s | ", formatted);
                    free(formatted);
                }
                
                if(FilterFlagArr[3]) {
                    char *formatted = ReturnStrXLength(DATE_WIDTH, ReturnJsonDate(Date));
                    printf("%s | ", formatted);
                    free(formatted);
                }
                
                if(FilterFlagArr[4]) {
                    char *formatted = ReturnStrXLength(LOCATION_WIDTH, ReturnJsonLocation(Date));
                    printf("%s | ", formatted);
                    free(formatted);
                }
                
                if(FilterFlagArr[5]) {
                    char *formatted = ReturnStrXLength(NOTES_WIDTH, ReturnJsonNotes(Date));
                    printf("%s", formatted);  // No " | " at the end
                    free(formatted);
                }
                
                printf("\n");
            }
            // if descending add -1 to i, else add 1, placed here incase never add anything to Dates?
            i += Descending ? -1 : 1;
            
        }
    }

    int DateHas(char* HasVal, int* InColFlagArr, cJSON* Date){
        //Clones values (to not effect printing), upper cases them. Then uses strstr to see if the HasValUpper (hasval)
        // is contained in the column field, sets flag to 1, free vars and returns 1 else only frees ColFieldUpper
                
        // Make uppercase copy of search term once - stops in-place capitalisation affecting prints
        char *HasValUpper = strdup(HasVal);
        StrToUpper(HasValUpper);
        
        if(InColFlagArr[0]){
            char *ColFieldUpper = strdup(ReturnJsonIdea(Date));
            StrToUpper(ColFieldUpper);
            if(strstr(ColFieldUpper, HasValUpper) != NULL){
                free(ColFieldUpper);
                free(HasValUpper);
                return 1;
            }
            free(ColFieldUpper);
        }
        
        if(InColFlagArr[1]){
            char *ColFieldUpper = strdup(ReturnJsonType(Date));
            StrToUpper(ColFieldUpper);
            if(strstr(ColFieldUpper, HasValUpper) != NULL){
                free(ColFieldUpper);
                free(HasValUpper);
                return 1;
            }
            free(ColFieldUpper);
        }
        
        if(InColFlagArr[2]){
            char *ColFieldUpper = strdup(ReturnJsonSeasonal(Date));
            StrToUpper(ColFieldUpper);
            if(strstr(ColFieldUpper, HasValUpper) != NULL){
                free(ColFieldUpper);
                free(HasValUpper);
                return 1;
            }
            free(ColFieldUpper);
        }
        
        if(InColFlagArr[3]){
            char *ColFieldUpper = strdup(ReturnJsonDate(Date));
            StrToUpper(ColFieldUpper);
            if(strstr(ColFieldUpper, HasValUpper) != NULL){
                free(ColFieldUpper);
                free(HasValUpper);
                return 1;
            }
            free(ColFieldUpper);
        }
        
        if(InColFlagArr[4]){
            char *ColFieldUpper = strdup(ReturnJsonLocation(Date));
            StrToUpper(ColFieldUpper);
            if(strstr(ColFieldUpper, HasValUpper) != NULL){
                free(ColFieldUpper);
                free(HasValUpper);
                return 1;
            }
            free(ColFieldUpper);
        }
        
        if(InColFlagArr[5]){
            char *ColFieldUpper = strdup(ReturnJsonNotes(Date));
            StrToUpper(ColFieldUpper);
            if(strstr(ColFieldUpper, HasValUpper) != NULL){
                free(ColFieldUpper);
                free(HasValUpper);
                return 1;
            }
            free(ColFieldUpper);
        }

        free(HasValUpper);
        return 0;
    }


    //Copmares the values specified by ValCol of two Date's dates and returns <0,0,1 if DatteA is bigger than or equal to or smaller than DateB, like strcmp as:
    //return x < 0
    //return x = 0
    //return x > 1 

    int DateCmp(char* DateA, char* DateB){
        //Value to store strcmps value to save on fn calls
        int result;
        //  d d / m m / y y
        //  0 1 2 3 4 5 6 7

        //check if the Year is the same (done by checking, chars @ [6] onwards)
        result = CompareDateComponent(&DateA[6], &DateB[6]);
        //If the years are not the same (i.e different), return result to define order
        if(result !=0){return result;}


        //If years are the same, check if months are different (done by checking chars 3 and 4)
        //To allow it to function inplace for strcmp need to change '/' to '\0'
        DateA[5] = '\0';
        DateB[5] = '\0';
        //check if the Year is the same (done by checking, chars @ [6] onwards)
        result = CompareDateComponent(&DateA[3], &DateB[3]);
        //result the "/" from "\0" for printing purposes before result returned
        DateA[5] = '/';
        DateB[5] = '/';
        
        //If the months are not the same (i.e different), return result to define order
        if(result !=0){return result;}


        //If months the same, check if days are different (done by checking chars 0 and 1)
        //To allow it to function inplace for strcmp need to change '/' to '\0'
        DateA[2] = '\0';
        DateB[2] = '\0';
        //check if the Year is the same (done by checking, chars @ [6] onwards)
        result = CompareDateComponent(&DateA[0], &DateB[0]);
        //result the "/" from "\0" for printing purposes before result returned
        DateA[2] = '/';
        DateB[2] = '/';
        
        //No matter what the result is, return it. 
        return result;
    }

    //Helper function that deals with "--" elements so they function like "X"
    int CompareDateComponent(const char* a, const char* b) {
        int isPlaceholderA = (a[0] == '-' && a[1] == '-');
        int isPlaceholderB = (b[0] == '-' && b[1] == '-');
        
        if(isPlaceholderA && isPlaceholderB) return 0;  // Both placeholders
        if(isPlaceholderA) return 1;   // A is placeholder, sort after B
        if(isPlaceholderB) return -1;  // B is placeholder, sort after A
        
        return strcmp(a, b);  // Normal string comparison
    }

    
    //Removes \n char from user input on datesv 
    void RemoveNewline(char* str) {
        str[strcspn(str, "\n")] = '\0';
    }

    char* IdeaValid(char* UserIdea){
        return UserIdea;
    }