package assignments2019.a2;
/*
 *   STUDENT NAME      :  Zhao Yi (Max) PI
 *   STUDENT ID        :260913518
 *   
 *   If you have any issues that you wish the T.A.s to consider, then you
 *   should list them here.   If you discussed on the assignment in depth 
 *   with another student, then you should list that student's name here.   
 *   We insist that you each write your own code.   But we also expect 
 *   (and indeed encourage) that you discuss some of the technical
 *   issues and problems with each other, in case you get stuck.    
	
	Kai Long Li
	Lin Xiao Zheng
	Jonathan Halimi
 *   
 */

import java.util.ArrayList;

public class MyBigInteger implements Comparable<MyBigInteger> { // We will cover what "implements Comparable" means
																// later

	/*
	 * 
	 * If the number has N digits, then it is represented is a polynomial:
	 * coefficients[N-1] base^{N-1} + ... coefficients[1] base^{1} + coefficients[0]
	 * where base has a particular value and the coefficients are in {0, 1, ... base
	 * - 1}
	 * 
	 * For any base and any positive integer, the representation of that positive
	 * integer as a sum of powers of that base is unique.
	 * 
	 * We require that the coefficient of the largest power is non-zero. For
	 * example, '354' is a valid representation (which we call
	 * "three hundred fifty four") but '0354' is not.
	 * 
	 */

	private int base;

	private ArrayList<Integer> coefficients;

	// Constructors

	MyBigInteger(int base) {

		// If no string argument is given, then this constructor constructs an empty
		// list of coefficients.

		this.base = base;
		coefficients = new ArrayList<Integer>();
	}

	/*
	 * The constructor builds a ArrayList of Integer objects where the integers are
	 * in [0,base), i.e. 0, 1, ..., base - 1. Note that the argument numberAsString
	 * only represents a base 10 number when the base is 10.
	 */

	MyBigInteger(String numberAsString, int base) throws Exception {
		int i;
		this.base = base;
		coefficients = new ArrayList<Integer>();
		if ((base < 2) || (base > 10)) {
			System.out.println("constructor error:  base must be between 2 and 10 (inclusive)");
			throw new Exception();
		}

		int len = numberAsString.length();
		for (int indx = 0; indx < len; indx++) {
			i = numberAsString.charAt(indx);

			/*
			 * ascii value of symbol '0' is 48, symbol '1' is 49, etc. So, to get the
			 * numerical value of '2', we subtract the character value of '0' (48) from the
			 * character value of '2' (50).
			 */

			if ((i >= 48) && (i - 48 < base))
				coefficients.add(0, Integer.valueOf(i - 48));
			else {
				System.out.println("constructor error:  all coefficients should be non-negative and less than base");
				throw new Exception();
			}
		}
	}

	/*
	 * This Constructor allows the input to be an int. The maximum value can
	 * therefore be 2^31-1. Note that the method assumes the input i is written in
	 * base 10. It then converts that value to a particular base.
	 */

	MyBigInteger(int i, int base) throws Exception {
		this.base = base;
		coefficients = new ArrayList<Integer>();

		if (i < 0) {
			System.out.println("constructor error: we are only dealing with non-negative integers");
			throw new Exception();
		} else if (i == 0)
			coefficients.add(Integer.valueOf(0));
		else {
			int m = i;
			while (m > 0) {
				coefficients.add(Integer.valueOf(m % base));
				m /= base;
			}
		}
	}

	public int getBase() {
		return base;
	}

	/*
	 * Normally we would not need this method, since coefficients are meant to be
	 * hidden from the user. The reason for having the method is that the Grader can
	 * use it to examine the number. Another way to do this would have been to make
	 * the field public.
	 */

	public ArrayList<Integer> getCoefficients() {
		return coefficients;
	}

	/*
	 * The plus method computes this.plus(b), that is, a+b where 'this' is a.
	 */

	public MyBigInteger plus(MyBigInteger second) throws Exception {

		// initialize the result as an empty list of coefficients

		MyBigInteger result = new MyBigInteger(this.base);

		if (this.base != second.base) {
			throw new Exception("Bases must be the same in an addition");
		}

		/*
		 * The plus method must not affect the numbers themselves. So let's just work
		 * with a copy (a clone) of the numbers.
		 */

		MyBigInteger firstClone = this.clone();
		MyBigInteger secondClone = second.clone();

		/*
		 * If the two numbers have a different polynomial order then pad the smaller one
		 * with zero coefficients.
		 */

		int diff = firstClone.coefficients.size() - second.coefficients.size();
		while (diff < 0) { // second is bigger
			firstClone.coefficients.add(0);
			diff++;
		}
		while (diff > 0) { // this is bigger
			secondClone.coefficients.add(0);
			diff--;
		}

		/*
		 * 'firstClone' and 'secondClone' have the same size. We add the coefficients
		 * term by term. If the last coefficient yields a carry, then we add one more
		 * term with the carry.
		 */

		int tmp;
		int carry = 0;

		for (int i = 0; i < firstClone.coefficients.size(); i++) {
			tmp = firstClone.coefficients.get(i) + secondClone.coefficients.get(i) + carry;
			result.coefficients.add(tmp % base);
			carry = tmp / base;
		}
		if (carry > 0)
			result.coefficients.add(carry); // carry always would have the value 1 in this case.

		return result;
	}

	/*
	 * Slow multiplication algorithm. 'this' is the multiplicand i.e. a*b =
	 * a+a+a+...+a (b times) where a is multiplicand and b is multiplier
	 */

	public MyBigInteger slowTimes(MyBigInteger multiplier) throws Exception {

		if (this.base != multiplier.base) {
			throw new Exception("Bases must be the same in slowTimes");
		}

		MyBigInteger prod = new MyBigInteger(0, this.base);
		MyBigInteger one = new MyBigInteger(1, this.base); // This is used to increment the counter.

		for (MyBigInteger counter = new MyBigInteger(0, this.base); counter.compareTo(multiplier) < 0; counter = counter
				.plus(one)) {
			prod = prod.plus(this);
		}
		return prod;
	}

	/*
	 * The times method is NOT be the same as what you learned in grade school which
	 * uses a temporary 2D table with space proportional to the square of the number
	 * of coefficients in the operands, i.e. N^2. The method here accumulates the
	 * result by adding each row of the table as it is computed. This method uses
	 * space that is proportional to the number of coefficients N. The
	 * multiplication algorithm will still take time O(N^2) however.
	 * 
	 * The method computes this.times(b) where 'this' is a.
	 */

	public MyBigInteger times(MyBigInteger multiplicand) throws Exception {

		if (this.base != multiplicand.base) {
			throw new Exception("Bases must be the same in times()");
		}

		// anything times 0 is 0, so if either 'this' or multiplicand are 0 then return
		// 0.
		MyBigInteger zero = new MyBigInteger(0, this.base);
		if ((this.compareTo(zero) == 0) || (multiplicand.compareTo(zero) == 0))
			return zero;

		// initialize product as an empty list of coefficients

		MyBigInteger product = new MyBigInteger(this.base);

		/*
		 * multiplicand x multiplier (this) ---------------
		 * 
		 * Note we use helper methods.
		 */

		MyBigInteger row;
		for (int i = 0; i < this.coefficients.size(); i++) {
			row = multiplicand.timesSingleDigit(this.coefficients.get(i)).timesBaseToThePower(i);
			product = product.plus(row);
		}
		return product;
	}

	/*
	 * 'this' (the caller) will be the multiplicand.
	 */

	private MyBigInteger timesSingleDigit(int singleDigit) throws Exception {

		// Assumes that 0 <= singleDigit < base.

		if ((singleDigit >= this.base) || (singleDigit < 0)) {
			throw new Exception("Single digit must be in {0, ..., this.base - 1}  in timeSingleDigit");
		}

		// if we multiply 'this' by 0, then the result is 0
		if (singleDigit == 0) {
			return new MyBigInteger(0, this.base);
		}

		int carry = 0;
		int tmp;

		// initialize prod as an empty list of coefficients

		MyBigInteger prod = new MyBigInteger(this.base);

		// multiply the single digit by each of the digits in this.coefficients
		// This is like a row in the grade school calculation.

		for (int i = 0; i < this.coefficients.size(); i++) {
			tmp = this.coefficients.get(i) * singleDigit + carry;
			prod.coefficients.add(tmp % this.base);
			carry = tmp / this.base;
		}
		if (carry > 0)
			prod.coefficients.add(carry);
		return prod;
	}

	/*
	 * The minus method computes this.minus(b) where 'this' is a, and a > b. If a <
	 * b, then it throws an exception.
	 * 
	 * The solution below uses variable names first and second rather than a and b.
	 * So we are computing first.minus(second)
	 */

	public MyBigInteger minus(MyBigInteger second) throws Exception {

		// initialize the result as an empty list of coefficients

		MyBigInteger result = new MyBigInteger(this.base);

		if (this.base != second.base) {
			throw new Exception("Bases must be the same in minus method.");
		}
		/*
		 * The minus method is not supposed to change the numbers. But the grade school
		 * algorithm sometimes requires us to "borrow" from a higher coefficient to a
		 * lower one. So we work with a copy (a clone) instead.
		 */

		MyBigInteger first = this.clone();

		// Verify 'this' >= second.

		if (this.compareTo(second) < 0) {
			throw new Exception("a.minus(b) requires that a >= b");
		}

		int i = 0; // coefficient position
		int diffCoef; // compute the difference of two coefficients this[i] - second[i]

		while (i < first.coefficients.size()) {

			/*
			 * Start from the least significant digit. For each i, check if 'second' has a
			 * term at position i. If yes, then need to take the difference at this
			 * position. That's where the bulk of work is. If no, then the coefficient at
			 * second is treated as 0 and just copy the i'th coefficient from first to the
			 * result.
			 */

			if (i < second.coefficients.size()) {

				diffCoef = first.coefficients.get(i) - second.coefficients.get(i);

				if (diffCoef >= 0)
					result.coefficients.add(Integer.valueOf(diffCoef));
				else {
					/*
					 * .. then we need to borrow from the next coefficient. But if the next
					 * coefficient holds a '0', then we need a sequence of borrows.
					 * 
					 * For example, suppose we are computing 30001 - 6. Then we essentially treat
					 * 30001 as 11 + 90 + 900 + 9000 + 2000. So we would want to rewrite the list of
					 * coefficients as (11, 9, 9, 9, 2).
					 */
					int j = i;
					while (first.coefficients.get(j + 1) == 0) {
						first.coefficients.set(j + 1, base - 1);
						j++;
					}
					first.coefficients.set(j + 1, Integer.valueOf(first.coefficients.get(j + 1) - 1));

					// Finally, compute the result for position i
					result.coefficients.add(Integer.valueOf(base + diffCoef));
				}
			} else
				result.coefficients.add(Integer.valueOf(first.coefficients.get(i)));
			i++;
		}

		/*
		 * In the case of say 100-98, we will end up with 002. Remove all the leading
		 * 0's of the result.
		 */

		while ((result.coefficients.size() > 1) & (result.coefficients.get(result.coefficients.size() - 1) == 0)) {
			result.coefficients.remove(result.coefficients.size() - 1);
		}
		return result;
	}

	/*
	 * Slow division algorithm is repeated subtraction, mentioned in lecture 1.
	 */

	public MyBigInteger slowdividedBy(MyBigInteger divisor) throws Exception {

		if (this.base != divisor.base) {
			throw new Exception("Bases must be the same in slowdividedBy method");
		}

		MyBigInteger one = new MyBigInteger(1, base);
		MyBigInteger quotient = new MyBigInteger(0, base);
		MyBigInteger remainder = this.clone();
		while (remainder.compareTo(divisor) >= 0) {
			remainder = remainder.minus(divisor);
			quotient = quotient.plus(one);
		}
		return quotient;
	}

	/*
	 * The dividedBy method divides 'this' by 'divisor' i.e. this.dividedBy(divisor)
	 * It returns the quotient and ignores the remainder.
	 */

	public MyBigInteger dividedBy(MyBigInteger divisor) throws Exception {

		

		// ADD YOUR CODE BELOW HERE
		

		if (this.base != divisor.base) {
			throw new Exception("Bases must be the same in dividedBy method");
		}

		MyBigInteger quotient = new MyBigInteger(this.base);

		MyBigInteger testDividend = new MyBigInteger(this.coefficients.get(coefficients.size() - 1), this.base); 
																									

		int i = 0;
		MyBigInteger tmpBigInteger = testDividend; 
		
//START off with finding the first "term" of the dividend to be initially divided
	//example: 467/34: test 4/34 NO test 46/34 YES & PROCEED
		
		while (coefficients.size() >= i + 2) { 

			if (testDividend.compareTo(divisor) == -1) {
				tmpBigInteger = testDividend;
				testDividend.invertCoeff();
				testDividend.coefficients.add(this.coefficients.get(coefficients.size() - (i + 2)));
				testDividend.invertCoeff();

			}
			i++;

		}

		if (testDividend.compareTo(divisor) == 1) { // now that the test dividend is big enough, take the before last
													// entry to make sure testDividend does not overshoot
			testDividend = tmpBigInteger;

		}

		else if (testDividend.compareTo(divisor) == -1) { //CASE: the entire dividend is not divisible; return (0_this.base) as the MyBigInteger answer

			quotient.coefficients.add(0);
			return quotient;
		}

		int nbOfCarry = this.coefficients.size() - testDividend.coefficients.size(); // number of times we have to carry
																						// down an integer from the
																						// divisor (and repeat
																						// slowDivision) TO BE USED IN
																						// FOR LOOP
		MyBigInteger innerResult = testDividend;
		//innerResult is the term that gets manipulated from: the initial dividend to: the dividend to be divided by in the next step/next carry

		quotient = testDividend.slowdividedBy(divisor);
		//QuotientTMP is a "partial" quotient: it's the part of the quotient involved with the manipulation of innerResult
		MyBigInteger quotientTMP = quotient.clone();
		
		boolean pencilCase = false; //boolean to see whether the last slowDivision was successful or not: to check if we have to carry an extra term down

		for (int pi1 = 0; pi1 < nbOfCarry; pi1++) { // main loop that basically does the small divisions with the broken
													// down steps
			 
			if (innerResult.compareTo(quotientTMP.times(divisor)) >=0) {
				pencilCase  = true;

				innerResult = (innerResult.minus(quotientTMP.times(divisor)));
				//innerResult keeps getting manipulated

				if (innerResult.compareTo(new MyBigInteger(0, this.base)) == 0) {

					innerResult.coefficients.clear();
				}

				innerResult.invertCoeff();
				innerResult.coefficients.add(
						this.coefficients.get(this.coefficients.size() - testDividend.coefficients.size() - (1 + pi1))); // bigger
																															// inner
																															// result
				innerResult.invertCoeff();
				
				quotientTMP.coefficients.clear();

				quotient.invertCoeff();
				quotientTMP.invertCoeff();
				
				for (int pi2 = 0; pi2 < 1; pi2++) { // this for loop adds the current "partial" quotient to the actual
													// quotient

					quotient.coefficients.add(innerResult.slowdividedBy(divisor).coefficients.get(pi2));
					quotientTMP.coefficients.add(innerResult.slowdividedBy(divisor).coefficients.get(pi2));

				}

				quotient.invertCoeff();
				quotientTMP.invertCoeff();

			} else if (innerResult.compareTo(quotientTMP.times(divisor)) != 1) {
				
				quotient.invertCoeff();
				quotientTMP.invertCoeff();

				if (pencilCase) { 
					quotient.coefficients.add(0);
				
				quotientTMP.coefficients.add(0);
				}

				quotient.invertCoeff();
				quotientTMP.invertCoeff();
							}
					}
		return quotient;
		// ADD YOUR CODE ABOVE HERE.
	}

	private void invertCoeff() { //inverts coeff to make sure the MyBigInteger.coefficients are being added properly

		ArrayList<Integer> invertedInteger = new ArrayList<Integer>(this.coefficients.size());
		for (int w = this.coefficients.size() - 1; w >= 0; w--) {
			invertedInteger.add(this.coefficients.get(w));

		}

		this.coefficients = invertedInteger;
	}

	/*
	 * The convert method converts between two bases that are each in {2, ..., 10}
	 * The convert method specifies the new base that you are converting into. The
	 * subtlety of the convert method is that it uses operations in this.base This
	 * can be tricky since if you are converting into a higher base then you have to
	 * consider that this.dividedBy() is performed in this.base.
	 * 
	 */

	public MyBigInteger convert(int newBase) throws Exception {

		//MyBigInteger remainder = this.clone();

		MyBigInteger result = new MyBigInteger(newBase);

		// ADD YOUR CODE BELOW HERE

		// MyBigInteger remainderSecond = remainder.clone();

		if (newBase == this.base) { 
			return this;
			
		} else if (newBase < this.base) {//higher to lower
			
			result = this.specificConvert(newBase); //check method below
			return result;


		} else if (newBase > this.base) {//lower to higher
			//first convert everything to base 10
			MyBigInteger baseBigInt = new MyBigInteger(this.base, 10);

			MyBigInteger convertedTo10 = new MyBigInteger(10);
			
			
			for(int i = 0; i<this.coefficients.size(); i++) {
			
				convertedTo10 = convertedTo10.plus(new MyBigInteger(coefficients.get(i), 10).times(pow(baseBigInt, i))); //pow method overloads for MyBigInteger entries;
			}
			
			System.out.println("converttot10 "+ convertedTo10);
			
			//then convert from base 10 to whatever base (notice base 10 is the biggest therefore converting from higher to lower with specificConver
			
			result = convertedTo10.specificConvert(newBase);
			return result;
			}
			
		// ADD YOUR CODE ABOVE HERE

		return result;
	}
	
		public MyBigInteger specificConvert(int newBase) throws Exception { //converts strictly from higher base to lower base
		
		
		MyBigInteger remainder = this.clone();

		MyBigInteger result = new MyBigInteger(newBase);
		MyBigInteger integerOfBase = new MyBigInteger(newBase, this.base);

		while (remainder.compareTo(new MyBigInteger(0, this.base)) != 0) {

			MyBigInteger remainderSecond = (remainder.mod(integerOfBase));
			result.coefficients.add(remainderSecond.coefficients.get(0));
			remainder = remainder.dividedBy(integerOfBase);
		}
		
		return result;
	}
	

	private static MyBigInteger pow(MyBigInteger b, int a) throws Exception { //basically overrides the pow() math thing to accommodate for MyBigInteger
		
		MyBigInteger result = new MyBigInteger(1, b.getBase());
		for(int w = 0; w<a; w++) {
			result = result.times(b);
	}
	return result;
}
	
	public ArrayList<MyBigInteger> primeFactors() throws Exception {

		ArrayList<MyBigInteger> factors = new ArrayList<MyBigInteger>();
		MyBigInteger thisBigInteger = this.clone();

		// ADD YOUR CODE BELOW HERE
		
		
		MyBigInteger sqrted = bigSquareRoot(this).plus( new MyBigInteger(1,this.base));
		String s = "";
		for(Integer entry : sqrted.coefficients) {
			s=entry.toString()+s;
		}
		int sqrtedConverted = 2147483647;		//defaults sqrtedConverted to the biggest possible integer (if parseint doesnt work)
		
	try {
			sqrtedConverted = Integer.parseInt(s, this.base);	//preference: having the sqrted converted to be the integer entry of the mybigint
	}
	catch (NumberFormatException e){
		
	}
		
		MyBigInteger zero = new MyBigInteger(0, this.base); 
		
		
		
		MyBigInteger two = new MyBigInteger(2, this.base);
		
		while(thisBigInteger.mod(two).compareTo(zero)==0) { //FIRST STEP do all the 2 prime entries to avoid case errors AND to double the speed of the next for loop
															//I get to increment i by 2 now that i know that all even numbers wont be primes
			
			factors.add(two);
			thisBigInteger = thisBigInteger.dividedBy(two);
			
			
		}
		
		for(int i=3; i <= sqrtedConverted; i+=2) { //check prime for all odd numbers (even cases were taken care of above)
			
			
			
			
			MyBigInteger currentBigInteger = new MyBigInteger(i, this.base);
			
			
				if(thisBigInteger.mod(currentBigInteger).compareTo(zero)==0) {
									
					factors.add(currentBigInteger);
				
					thisBigInteger = thisBigInteger.dividedBy(currentBigInteger);
			
				
				if(thisBigInteger.compareTo(currentBigInteger.times(currentBigInteger))<0) { //exit condition: we know for sure there's no more primes beyond this point
					break;
				}
				
					i-=2;
		
				}
			
		
			if(thisBigInteger.compareTo(new MyBigInteger(1, this.base))==0) { //exit condition: if thisBigInteger = 1, there could be no more primes left
				break;
			}
				
		}
		
		if(thisBigInteger.compareTo(zero)!=0 && thisBigInteger.compareTo(new MyBigInteger(1, this.base))!=0) { //edge case treatment
			factors.add(thisBigInteger);
		}	
		

		// ADD YOUR CODE ABOVE HERE
		
		return factors;
	}

	private static MyBigInteger bigSquareRoot(MyBigInteger MBI) throws Exception { //square root of MyBigInteger
		
		MyBigInteger one = new MyBigInteger(1, MBI.base);
		MyBigInteger two = new MyBigInteger(2, 10);
		two = two.convert(MBI.base);
		
		MyBigInteger x = MBI.clone();
		MyBigInteger y = x.plus(one).dividedBy(two);
		
		while (y.compareTo(x)<0) {
			x=y.clone();
			MyBigInteger div = x.timesSingleDigit(2);
			y=x.times(x).plus(MBI).dividedBy(div);
			
		}
		
		return x;
		
	}
	// ---------------- HELPER METHODS --------------------------

	/*
	 * The mod method divides 'this' by 'divisor' and returns the remainder.
	 */

	public MyBigInteger mod(MyBigInteger divisor) throws Exception {

		if (this.base != divisor.base) {
			throw new Exception("Bases must be the same in mod method");
		}
		return this.minus(this.dividedBy(divisor).times(divisor));
	}

	@Override
	public MyBigInteger clone() {

		// For technical reasons that don't interest us here, this method
		// has to be declared public (not private).

		MyBigInteger copy = new MyBigInteger(this.base);
		for (int i = 0; i < this.coefficients.size(); i++) {
			copy.coefficients.add(Integer.valueOf(this.coefficients.get(i)));
		}
		return copy;
	}

	/*
	 * The a.compareTo(b) method returns -1 if a < b, it returns 0 if a == b, and it
	 * returns 1 if a > b.
	 * 
	 * Assumes that numbers have valid representation e.g. no leading 0's.
	 */

	public int compareTo(MyBigInteger second) {

		// if this < second, return -1
		// if this > second, return 1
		// otherwise they are equal and return 0

		if (this.base != second.base) {
			throw new RuntimeException("Bases must be the same in compareTo method");
		}

		/*
		 * Assume maximum degree coefficient is non-zero (except for case that one of
		 * the numbers is 0). Then, if two numbers have different maximum degree, it is
		 * easy to decide which is larger.
		 */

		int diff = this.coefficients.size() - second.coefficients.size();
		if (diff < 0)
			return -1;
		else if (diff > 0)
			return 1;
		else {

			/*
			 * If two numbers have the same maximum degree, then it is a bit trickier to
			 * decide which number is larger. You need to compare the coefficients, starting
			 * from the largest and working toward the smallest until you find coefficients
			 * that are not equal.
			 */

			boolean done = false;
			int i = this.coefficients.size() - 1;
			while (i >= 0 && !done) {
				diff = this.coefficients.get(i) - second.coefficients.get(i);
				if (diff < 0) {
					return -1;
				} else if (diff > 0)
					return 1;
				else {
					i--;
				}
			}
			return 0; // if all coefficients are the same, so numbers are equal.
		}
	}

	/*
	 * computes 'this' * base^n Note that it modifies the number, so if you don't
	 * want that, then clone the number first.
	 */

	private MyBigInteger timesBaseToThePower(int n) throws Exception {

		if (n < 0) {
			throw new Exception("timesBaseToThePower requires that the power (exponent) >= 0");
		}

		if (this.compareTo(new MyBigInteger(0, base)) == 0)
			return this;

		MyBigInteger thisShifted = new MyBigInteger(base);
		// First make a list of n 0's.
		for (int i = 0; i < n; i++) {
			thisShifted.coefficients.add(Integer.valueOf(0));
		}

		// Then add all the coefficents of 'this' after the n 0's.
		for (int i = 0; i < this.coefficients.size(); i++) {
			thisShifted.coefficients.add(this.coefficients.get(i));
		}
		return thisShifted;
	}

	/*
	 * Returns a string with coefficients in the reverse order which is the natural
	 * format for people to reading numbers, i.e. people want to read a[N-1], ...
	 * a[2] a[1] a[0].
	 */

	@Override
	public String toString() {
		String s = new String();
		for (Integer coef : coefficients) // Java enhanced for loop
			s = coef.toString() + s; // Append each successive coefficient.
		return "(" + s + ")_" + base;
	}

	public static ArrayList<MyBigInteger> primesToN(int n, int base) throws Exception {

		// Sieve of Eratosthenes algorithm
		//
		// Code here is modified from
		// https://introcs.cs.princeton.edu/java/14array/PrimeSieve.java.html

		// initially assume all integers are prime

		boolean[] isPrime;
		ArrayList<MyBigInteger> listOfPrimes = new ArrayList<MyBigInteger>();

		isPrime = new boolean[n + 1];
		for (int i = 2; i <= n; i++) {
			isPrime[i] = true;
		}

		// mark non-primes <= n using Sieve of Eratosthenes
		for (int factor = 2; factor * factor <= n; factor++) {

			// If factor is prime, then mark multiples of factor as nonprime
			// It suffices to consider multiples factor, factor+1, ..., n/factor.
			if (isPrime[factor]) {
				for (int j = factor; factor * j <= n; j++) {
					isPrime[factor * j] = false;
				}
			}
		}
		for (int i = 2; i <= n; i++) {
			if (isPrime[i] == true)
				listOfPrimes.add(new MyBigInteger(i, base));
		}
		return listOfPrimes;
	}
}