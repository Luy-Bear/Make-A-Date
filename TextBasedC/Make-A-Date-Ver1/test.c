#include <stdio.h>


void QuickSortDates(int LowerLim, int UpperLim, int Dates[]){
    // Check if i can do median of 3...
    //if one do nothing, leave as cant sort anymore. 
    if(!(LowerLim >= UpperLim)){
        printf("Quick Sort Called\n");
    
        //if two sort and return
        if(UpperLim-LowerLim == 1){
            
            printf("2 Arr Length Reached\n");
            //If lower lim date is bigger than upper lim (a>b), swap using temp value, else its in order and do nothing
            if(Dates[LowerLim] > Dates[UpperLim]){
                int temp = Dates[UpperLim];
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
            
            if(Dates[LowerLim] < Dates[(LowerLim+UpperLim)/2]){ //a < b but where is c?
                if(Dates[(LowerLim+UpperLim)/2] < Dates[UpperLim]){PivotIndex = (LowerLim+UpperLim)/2;}//a b c
                else if((Dates[UpperLim])< Dates[LowerLim]){PivotIndex = LowerLim;}//c a b
                else{PivotIndex = UpperLim;}//a c b
                    
            }
            else{//b < a but where is c?
                if(Dates[LowerLim] < Dates[UpperLim]){PivotIndex = LowerLim;}//b a c
                else if(Dates[(LowerLim+UpperLim)/2] < Dates[UpperLim]){PivotIndex = UpperLim;} //b c a
                else{PivotIndex = (LowerLim+UpperLim)/2;}//c b a
            }

            //Now have pivot @ index stored with PivotIndex
            int LPointer = LowerLim, RPointer = UpperLim-1; //Rpointer is -1 from limit to ignore pivot

            //check if pivot is last elem, if not swap them
            if(PivotIndex != UpperLim){
                int temp = Dates[PivotIndex];
                Dates[PivotIndex] = Dates[UpperLim];
                Dates[UpperLim] = temp;
                PivotIndex = UpperLim;  // NOW PIVOT IS ON THE FAR RIGHT NEED TO ADJUST THE PIVOT INDEX POINTER VALUE TO REFLECT THIS!
            }


            //Begin to order the Dates with pointers passed....
            while(LPointer <= RPointer){
                if(Dates[LPointer] > Dates[PivotIndex]){ //Is LPointer > Pivot Val (needs changing)
                    if(Dates[RPointer] < Dates[PivotIndex]){ //Is Right value < pointer value (needs changing)
                        //Swap the pair round
                        int temp = Dates[RPointer];
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
                    if(Dates[RPointer] > Dates[PivotIndex]) //Value Doesnt need changing, so move pointer
                        {//shift R pointer by -1
                            RPointer--;
                        }   
                    // (else needs to swap but cant becuase L doesnt so keep pointer the same)
                }
            
            }
            //Ordering done, now tneed to place pivot in place.
            //LPointer points at value that is bigger than the pivot so swap this with pivot which is at the upper index.    
            int temp = Dates[UpperLim];
            Dates[UpperLim] = Dates[LPointer]; //Putes LPointer val on right side
            Dates[LPointer] = temp;            //Puts Pivot in place
            //Recursive call on remaining sectinos of list
            QuickSortDates(LowerLim, LPointer-1, Dates); //LPointer, just swaped the last "bigger than pivot" with the far right elem (pivot), so Lpointer points at pivot. So do one less than pivot
            QuickSortDates( LPointer+1, UpperLim, Dates); //As LPointer points at pivot, use the next obj in array as the lower lim.
        }
    }
    return;        
}

int main(int argc, char *argv[]){
 
    int Dates[6] = {86,  888, 80, 292, 63, 419};

    QuickSortDates(0, 5, Dates);

    printf("\n\n\n\n");

    for(int i = 0; i < 6; i++){
        printf("%d\n", Dates[i]);
    }

    


    return 0;
}