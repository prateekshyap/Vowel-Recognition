/*Try increasing the window width well to read smoothly.
Notepad also shows the line numbers at the bottom.*/

=======================================================================
				Contents
=======================================================================

Names							Line number
-----							-----------

Assumptions and calculations					18
Functions and their Functionalities				51
How to create the input files 					97
Testcases description						108
Instructions to execute the code 				124

=======================================================================
			Assumptions and calculations
=======================================================================

1. DC Shift
-----------

DC shift is found out by taking the average of the sample data where
microphone is turned off.
(I have considered the DC shift to be 0 if the average is less than
0.0005, since this value is too small.)

2. Normalization Factor
-----------------------

Normalization factor is found out by dividing the average of the two
extreme values by 5000. For example - if the maximum value is 12500 and
the minimum value is -12600 then normalization factor will be
5000/((12500+12600)/2) i.e. 0.398406 .
(To make things simpler, I am taking only one digit after decimal point.
That means 0.398406 becomes 0.3 . It doesn't affect the values much.
It also ensures that the range remains 5000 to -5000 since we are
reducing the value from 0.398406 to 0.3 .)

3. Stable frames searching
--------------------------

While reading the input file, the point at which the amplitude value
goes beyond the range -1500 to 1500 (i.e. either smaller than -1500 or
larger than 1500) next 2000 amplitude values are skipped and next 320x5
values are taken. This process is giving good results. (100% accuracy for
all vowels except /i/ which has 80% accuracy)

=======================================================================
		Functions and their Functionalities
=======================================================================

1. float findDCShift(char *fileName)
------------------------------------

This function is finding out the DC shift from <fileName> file.

2. float getNormalizationFactor(char* fileName, float range)
------------------------------------------------------------

This function finds out the normalization factor for the given speech
data.

3. void writeToFile(double * arr, char * fileName, int size)
------------------------------------------------------------

This function is writing the data of arr into fileName file.

4. double * getRValues(double * x, int sampleSize, int p)
---------------------------------------------------------

This function is finding out the R values using the auto corelation
formula.

5. double * getAValues(double * R, int sampleSize, int p)
---------------------------------------------------------

This function is finding out the A values using Levinson Durbin's
Algorithm.

6. double * getCValues(double * A, int sampleSize, int p, double r0)
--------------------------------------------------------------------

This function is calculating C_i values for 1 <= m <= p.

7. int _tmain(int argc, _TCHAR* argv[])
---------------------------------------

This is the main function. Here other functions are called sequentially
as per the requirement. First the validation is done for the given data
an it is written into a file for reference. Then training is done for
first 10 samples followed by testing for the next 10 samples. Tokhura
distance is taken into consideration for finding out the vowel spoken.

=======================================================================
			How to create the input files
=======================================================================

1. Speech sample
----------------

This can be recorded by speaking the vowels and can be named as per
requirement. It is better to follow some convention so that the string
concatenation features can be used.

=======================================================================
			Testcases description
=======================================================================

The convention followed is - a few fixed digits followed by vowel name
followed by count e.g. 214101037_a_04. So for each iteration only the
vowel name and count will be changed. This format can be given in the
array at line number 193 to 195. All the other informations such as
sample size, p, q, number of alphabets, number of speech samples etc
can be changed as per requirement from line number 185 to 216. The
variables on the next lines shouldn't be altered.

At line numbers 287-289, 348, 371-373, 424 must be changed according to
the file name convention. These lines replace the vowel and the file
count for each iteration. 

=======================================================================
			Instructions to execute the code 
=======================================================================

1. The project should be opened in visual studio.

2. The variables which should be decided before execution are listed
separately within main function at line number - 185 to 216.

3. If the naming convention of the files are being changed, then proper
changes to some variables should be made.

4. To see the console window, a breakpoint should be put at line
number - 478. For this, the shortcut is to place the cursor at 478 and
press f9 key. (Fn+f9 for some systems)

5. f7 can be pressed for building the project and f5 should be
pressed for execution.

6. Output can be observed in the console window.

7. The output file containing the same information as the console window
will be in the same directory where the program is present.

/*
Giving 80%+ accuracy for others' speech samples also ;)
*/
