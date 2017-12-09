int main()
{
	int arr[] = {0,5};
	int testInt = 10008;
	double testDouble = 1.000808;
	float testFloat = 10008.86;
	bool testBool = false;
	while ( testInt <= testDouble  && ( !testBool || testFloat > 0 ) )
	{
		testInt += 1;
		testDouble -= testInt;
		testBool = false;
	}
	testBool = true;
	if ( testBool ){
		testInt = 10;
	}
	else{
		testDouble += 8888.888;
	}
	return 0;
}
