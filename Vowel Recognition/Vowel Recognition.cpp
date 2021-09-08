// YesNoDetection.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <string.h>

float findDCShift(char* fileName)
{
	char line[100]; //string to read lines from the input file
	int x = 0; //stores the x_i values
	long noOfSamples = 0; //stores the total number of samples
	float DCShift = 0.0f; //to store the DC shift value

	if (fileName == NULL) return 0; //return 0 if no DC shift test file is given

	FILE* file = fopen(fileName,"r"); //open input file in read mode

	//till end of fileis not reached
	while(!feof(file))
	{
		fgets(line, 100, file); //read one line from the file
		if(!isalpha(line[0])) //if it is a numerical data
		{
			x = atoi(line); //convert it into integer
			DCShift += x; //add it to dc shift
			++noOfSamples; //increment sample count
		}
		if (noOfSamples == 2000)
			break;
	}

	fclose(file); //close the file
	DCShift /= noOfSamples; //find out the average
	if (abs(DCShift) < 0.0005) return 0; //if average is less than 0.0005 then return 0
	return DCShift; //otherwise return DC Shift value
}

float getNormalizationFactor(char* fileName, float range)
{
	float result = 0.0f; //to store the final result
	char line[100]; //string to read lines from the input file
	/*
	data stores the data read from the file
	maxData and minData store the maximum and minimum data point respectively
	*/
	int data = 0, maxData = 0, minData = 0;

	//some variables used to truncate the float result
	int shift = 0, temp = 0;
	float copy = 0.0f;

	FILE* file = fopen(fileName,"r"); //open input file in read mode

	if (file == NULL)
		printf("File not found");
	else
	{
		//till the end of file is not reached
		while (!feof(file))
		{
			fgets(line,100,file); //read next line
			if (!isalpha(line[0])) //if it is numerical
			{
				data = atoi(line); //convert into integer
				if (data > maxData) maxData = data; //update maxData
				if (data < minData) minData = data; //update minData
			}
		}

		minData = abs(minData); //get the absolute value of the minimum
		result = range/((maxData+minData)/2); //get the normalization factor
		
		//truncate the result to one decimal digit
		copy = result; //store the result in copy
		while (copy < 1) //till copy is less than 1
		{
			++shift; //increment shift
			copy *= 10; //keep multiplying copy with 10
		}
		temp = (int)copy; //store the integer part of copy in temp
		copy -= temp; //subtract temp from copy
		while (shift-- > 0) //reverse the shift process
			copy /= 10;
		/*the above process helps in getting all the digits except the first decimal digit*/
		result -= copy; //subtract copy from result
		/*this will ensure that only one digit remains after decimal point*/
	}

	fclose(file); //close the file

	//return the result
	return result;
}

void writeToFile(double * arr, char * fileName, int size)
{
	int i = 0;
	FILE * file = fopen(fileName,"w");
	for (i = 0; i <= size; ++i)
	{
		fprintf(file,"%lf",arr[i]);
		fprintf(file,"\n");
	}
	fclose(file);
}

double * getRValues(double * x, int sampleSize, int p)
{
	double * R = new double[13];
	int i = 0, j = 0;
	for (i = 0; i <= p; ++i)
	{
		R[i] = 0;
		for (j = 0; j < sampleSize-i; ++j)
			R[i] += x[j]*x[i+j];
	}
	return R;
}

double * getAValues(double * R, int sampleSize, int p)
{
	//following Durbin's Algorithm
	int i = 0, j = 0;
	double * A = new double[p];
	double * E = new double[p+1];
	double * K = new double[p+1];
	double ** tempA = new double*[p+1];

	//initializing everything to 0
	for (i = 0; i < p+1; ++i)
		tempA[i] = new double[p+1];
	for (i = 0; i < p+1; ++i)
	{
		E[i] = 0;
		K[i] = 0;
	}
	for (i = 0; i < p+1; ++i)
		for (j = 0; j < p+1; ++j)
			tempA[i][j] = 0;

	//Algorithm steps
	E[0] = R[0];
	for (i = 1; i <= p; ++i)
	{
		K[i] = 0;
		for (j = 1; j < i; ++j)
			K[i] += tempA[i-1][j]*R[i-j];
		K[i] = R[i]-K[i];
		K[i] /= E[i-1];
		tempA[i][i] = K[i];
		for (j = 1; j < i; ++j)
			tempA[i][j] = tempA[i-1][j]-(K[i]*tempA[i-1][i-j]);
		E[i] = (1-(K[i]*K[i]))*E[i-1];
	}
	for (i = 1; i <= p; ++i)
		A[i-1] = tempA[p][i];
	return A;
}

double * getCValues(double * A, int sampleSize, int p, double r0)
{
	int i = 0, j = 0;
	double * C = new double[p+1];
	C[0] = log(r0*r0);
	for (i = 1; i < p+1; ++i)
	{
		C[i] = 0;
		double m = i;
		for (j = 1; j < i; ++j)
		{
			double k = j;
			C[i] += (k/m)*C[j]*A[i-j-1];
		}
		C[i] += A[i-1];
	}
	return C;
}

int _tmain(int argc, _TCHAR* argv[])
{
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/*Variables Declaration*/
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	/*testFileName is the input file for sample data given
	testRFileName, testAFileName and testCFileName store the results, for reference
	*/
	char * testFileName = "test.txt", * testRFileName = "testRCalc.txt", * testAFileName = "testACalc.txt", * testCFileName = "testCCalc.txt";
	char trainingFileName[] = {'t','r','a','i','n','i','n','g','/','2','1','4','1','0','1','0','3','7','_','a','_','0','1','.','t','x','t','\0'}; //to be modified in each iteration
	char testingFileName[] = {'t','e','s','t','i','n','g','/','2','1','4','1','0','1','0','3','7','_','a','_','0','1','.','t','x','t','\0'}; //to be modified in each iteration
	char referenceFileName[] = {'t','e','s','t','i','n','g','/','r','e','f','_','a','.','t','x','t','\0'}; //to be modified in each iteration
	/*sampleSize stores N
	p and q correspond to their actual norations
	sequenceSize denotes the total number of samples per alphabet
	alphabetsSize denotes the number of alphabets being considered (in our case it is the 5 vowels)
	trainingSize denotes the number of samples taken for training per alphabet
	testingSize denotes the number of samples taken for testing per alphabet
	noOfFrames denotes the number of frames being considered
	skipCount and skipReset are used to skip those many number of data points from the beginning of the speech
	*/
	int sampleSize = 320, p = 12, q = 12, sequenceSize = 20, alphabetsSize = 5, trainingSize = 10, noOfFrames = 5, testingSize = sequenceSize-trainingSize, skipCount = 2000, skipReset = skipCount, minAmp = -1500, maxAmp = 1500;
	float range = 5000.0f; //indicates the highest and lowest amplitude
	/*testData stores the samples from the given test file
	trainingData stores the samples during training
	testingData stores the samples during testing
	R, A, C store R_i, A_i, C_i values respectively
	distances stores the Tokhura distances
	*/
	double * testData = NULL, * trainingData = new double[sampleSize], * testingData = new double[sampleSize], * R = NULL, * A = NULL, * C = NULL, * distances = new double[alphabetsSize];
	double w[] = {1.0,3.0,7.0,13.0,19.0,22.0,25.0,33.0,42.0,50.0,56.0,61.0}; //given Tokhura weights
	/*sequence numbers and alphabets stored in string format*/
	char * sequence[20] = {"01","02","03","04","05","06","07","08","09","10","11","12","13","14","15","16","17","18","19","20"}, * alphabets[5] = {"a","e","i","o","u"};
	/*i,j,v,s,f,t are loop variables
	minIndex stores the index with minimum Tokhura distance*/
	int i = 0, j = 0, v = 0, s = 0, f = 0, t = 0, minIndex = 0;
	/*x stores data values read from file
	cRef stores the average C_i values from the reference files
	cFin stores the average C_i values from the testing files
	min stores the minimum Tokhura distance*/
	double x = 0, cRef = 0, cFin = 0, min = 999999999999999;
	char line[100]; //used to store the lines read from a file
	float dcShift = 0.0f, normalizationFactor = 0.0f;
	FILE * tempFile = NULL; //to open files
	double ** cFinal = new double * [noOfFrames]; //to store the final iteration C_i values
	for (i = 0; i < noOfFrames; ++i)
		cFinal[i] = new double[p];
	for (i = 0; i < noOfFrames; ++i)
		for (j = 0; j < p; ++j)
			cFinal[i][j] = 0;
	double ** cReference = new double * [noOfFrames]; //to store the C_i values from the reference files
	for (i = 0; i < noOfFrames; ++i)
		cReference[i] = new double[p];
	for (i = 0; i < noOfFrames; ++i)
		for (j = 0; j < p; ++j)
			cReference[i][j] = 0;
	double *** cTotal = new double ** [trainingSize]; //to store the C_i values for each alphabet
	for (i = 0; i < trainingSize; ++i)
		cTotal[i] = new double * [noOfFrames];
	for (i = 0; i < trainingSize; ++i)
		for (j = 0; j < noOfFrames; ++j)
			cTotal[i][j] = new double[p];
	for (i = 0; i < trainingSize; ++i)
		for (j = 0; j < noOfFrames; ++j)
			for (s = 0; s < p; ++s)
				cTotal[i][j][s] = 0;

	i = 0; j = 0; s = 0; //resetting loop variables

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/*Validation of sample data*/
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	//reading sample data into array
	FILE *testFile = fopen(testFileName,"r"); //open test file
	testData = new double[sampleSize]; //declare the array
	while (!feof(testFile)) //till eof
	{
		fgets(line,100,testFile); //read a line
		testData[i++] = atof(line); //convert into float and store in array
	}
	fclose(testFile); //close file

	//getting R values and writing into a file
	R = getRValues(testData,sampleSize,p);
	writeToFile(R,testRFileName,p);

	//getting A values and writing into a file
	A = getAValues(R,sampleSize,p);
	writeToFile(A,testAFileName,p-1);

	//getting C values and writing into a file
	C = getCValues(A,sampleSize,p,R[0]);
	writeToFile(C,testCFileName,p);

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/*Training*/
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	for (v = 0; v < alphabetsSize; ++v) //indicates alphabet number
	{
		for (s = 0; s < trainingSize; ++s) //indicates sequence number
		{
			/*generating the new file name*/
			trainingFileName[19] = alphabets[v][0];
			trainingFileName[21] = sequence[s][0];
			trainingFileName[22] = sequence[s][1];
			/*getting dc shift and normalization factor*/
			dcShift = findDCShift(trainingFileName);
			normalizationFactor = getNormalizationFactor(trainingFileName,range);
			/*finding out the starting point of the stable part*/
			tempFile = fopen(trainingFileName,"r");
			while (!feof(tempFile)) //till eof
			{
				fgets(line,100,tempFile); //read a line
				x = atof(line); //convert into double
				if (x > maxAmp || x < minAmp) //when amplitude goes beyond these values
					break;
			}
			for (f = 0; f < noOfFrames; ++f) //indicates frame number
			{
				i = 0; //reset i
				while (!feof(tempFile)) //till eof
				{
					fgets(line,100,tempFile); //read a line
					if (skipCount-- >0) //skip 2000 values
						continue;
					trainingData[i] = atof(line); //convert into float and store in array
					trainingData[i] -= dcShift; //apply dc shift
					trainingData[i] *= normalizationFactor; //do normalization
					if (++i == sampleSize) break; //read 320 samples and break
				}
				skipCount = skipReset;

				//getting R values
				R = getRValues(trainingData,sampleSize,p);

				//getting A values
				A = getAValues(R,sampleSize,p);
				/*for (j = 0; j < p; ++j)
					A[j] *= -1;*/

				//getting C values
				C = getCValues(A,sampleSize,p,R[0]);
				
				//raised sine window on C
				for (j = 1; j <= p; ++j)
					C[j] *= 1+((q/2)*sin((3.14*j)/q));

				//storing C in cTotal
				for (j = 1; j <= p; ++j)
					cTotal[s][f][j-1] = C[j];
			}
			fclose(tempFile); //close file
		}
		for (f = 0; f < noOfFrames; ++f) //indicates number of frames
		{
			for (t = 0; t < trainingSize; ++t) //indicates training size
				for (j = 0; j < p; ++j) //indicates size of C
					cFinal[f][j] += cTotal[t][f][j]; //add values with respect to the files
			for (j = 0; j < p; ++j) //take the average
				cFinal[f][j] /= 10;
		}
		
		//write cFinal to file
		referenceFileName[12] = alphabets[v][0]; //change the file name
		tempFile = fopen(referenceFileName,"w"); //open in write mode
		for (f = 0; f < noOfFrames; ++f) //indicates number of frames
		{
			for (t = 0; t < p; ++t) //indicates p values
			{
				fprintf(tempFile,"%lf",cFinal[f][t]);
				fprintf(tempFile,"\n");
			}
		}
		fclose(tempFile); //close file
	}
	
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/*Testing*/
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	FILE * output = fopen("consoleOutput.txt","w");

	for (v = 0; v < alphabetsSize; ++v)
	{
		for (s = trainingSize; s < trainingSize+testingSize; ++s)
		{
			/*generating the new file name*/
			testingFileName[18] = alphabets[v][0];
			testingFileName[20] = sequence[s][0];
			testingFileName[21] = sequence[s][1];
			dcShift = findDCShift(testingFileName);
			normalizationFactor = getNormalizationFactor(testingFileName,range);
			tempFile = fopen(testingFileName,"r");
			while (!feof(tempFile)) //till eof
			{
				fgets(line,100,tempFile); //read a line
				x = atof(line);
				if (x > maxAmp || x < minAmp)
					break;
			}
			for (f = 0; f < noOfFrames; ++f) //indicates frame number
			{
				i = 0;
				while (!feof(tempFile))
				{
					fgets(line,100,tempFile); //read a line
					if (skipCount-- >0)
						continue;
					testingData[i] = atof(line); //convert into float and store in array
					testingData[i] -= dcShift;
					testingData[i] *= normalizationFactor;
					if (++i == sampleSize) break;
				}
				skipCount = skipReset;

				//getting R values and writing into a file
				R = getRValues(testingData,sampleSize,p);

				//getting A values and writing into a file
				A = getAValues(R,sampleSize,p);
				/*for (j = 0; j < p; ++j)
					A[j] *= -1;*/

				//getting C values and writing into a file
				C = getCValues(A,sampleSize,p,R[0]);
				
				//sine window on C
				for (j = 1; j <= p; ++j)
					C[j] *= 1+((q/2)*sin((3.14*j)/q));

				//storing C in cFinal
				for (j = 1; j <= p; ++j)
					cFinal[f][j-1] = C[j];
			}
			fclose(tempFile); //close file

			//find out Tokhura distance
			for (t = 0; t < alphabetsSize; ++t) //for each alphabet
			{
				distances[t] = 0; //initialize distance to 0
				referenceFileName[12] = alphabets[t][0]; //update the reference file name
				tempFile = fopen(referenceFileName,"r"); //open in read mode
				/*read and store the values in cReference*/
				for (f = 0; f < noOfFrames; ++f)
				{
					for (j = 0; j < p; ++j)
					{
						fgets(line,100,tempFile); //read a line
						cReference[f][j] = atof(line);
					}
				}
				fclose(tempFile); //close file
				for (j = 0; j < p; ++j) //for each p value
				{
					cRef = 0;
					cFin = 0;
					//keep adding the C_i values frame wise
					for (f = 0; f < noOfFrames; ++f)
					{
						cRef += cReference[f][j];
						cFin += cFinal[f][j];
					}
					/*divide by number of frames*/
					cRef /= noOfFrames;
					cFin /= noOfFrames;
					/*this gives the average C_i of 5 frames*/
					distances[t] += w[j]*(cFin-cRef)*(cFin-cRef); //find out the distance
				}
			}
			min = 999999999999999; //reinitialize min
			for (t = 0; t < alphabetsSize; ++t) //for each alphabet
			{
				if (distances[t] < min) //update min and minIndex
				{
					min = distances[t];
					minIndex = t;
				}
			}

			/*print the result to console*/
			printf("File taken - %s\n",testingFileName);
			printf("Recognized alphabet- %s\n",alphabets[minIndex]);
			for (j = 0; j < alphabetsSize; ++j)
				printf("%lf ",distances[j]);
			printf("\n\n");
			/*print the result to file*/
			fprintf(output,"File taken - %s\n",testingFileName);
			fprintf(output,"Recognized alphabet- %s\n",alphabets[minIndex]);
			for (j = 0; j < alphabetsSize; ++j)
				fprintf(output,"%lf ",distances[j]);
			fprintf(output,"\n\n");
		}
	}
	fclose(output); //close file
	printf("Breakpoint");
	return 0;
}