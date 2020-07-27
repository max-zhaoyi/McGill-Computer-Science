package assignments2019.a3posted;
import java.util.ArrayList;
import java.util.Iterator;

//Name: PI, Zhao Yi
//Student ID: 260913518
public class KDTree implements Iterable<Datum>{

    KDNode 		rootNode;
    int    		k;
    int			numLeaves;

    // constructor

    public KDTree(ArrayList<Datum> datalist) throws Exception {

        Datum[]  dataListArray  = new Datum[ datalist.size() ];

        if (datalist.size() == 0) {
            throw new Exception("Trying to create a KD tree with no data");
        }
        else
            this.k = datalist.get(0).x.length;

        int ct=0;
        for (Datum d :  datalist) {
            dataListArray[ct] = datalist.get(ct);
            ct++;
        }

        //   Construct a KDNode that is the root node of the KDTree.

        rootNode = new KDNode(dataListArray);
    }

    //   KDTree methods

    public Datum nearestPoint(Datum queryPoint) {
        return rootNode.nearestPointInNode(queryPoint);
    }


    public int height() {
        return this.rootNode.height();
    }

    public int countNodes() {
        return this.rootNode.countNodes();
    }

    public int size() {
        return this.numLeaves;
    }

    //-------------------  helper methods for KDTree   ------------------------------

    public static long distSquared(Datum d1, Datum d2) {

        long result = 0;
        for (int dim = 0; dim < d1.x.length; dim++) {
            result +=  (d1.x[dim] - d2.x[dim])*((long) (d1.x[dim] - d2.x[dim]));
        }
        // if the Datum coordinate values are large then we can easily exceed the limit of 'int'.
        return result;
    }

    public double meanDepth(){
        int[] sumdepths_numLeaves =  this.rootNode.sumDepths_numLeaves();
        return 1.0 * sumdepths_numLeaves[0] / sumdepths_numLeaves[1];
    }

    //-------------------  my helpers   ------------------------------
    private int[] minMax(Datum[] datum, int kdim) {
        
        int min = datum[0].x[kdim], max=min;
      
        
        int[] minMax = new int[2]; 
        for (Datum data : datum) {
            int dataPoint = data.x[kdim];
            if (dataPoint < min) {
                min = dataPoint;
            }
            if (dataPoint > max) {
                max = dataPoint;
            }
        }
        minMax[0] = min;
        minMax[1] = max;

        return minMax;
    }
          
    private static int maxValue(int[] partialDatum) { //maxValue of array

        int maxValueX = partialDatum[0];
        for (int i = 1; i < partialDatum.length; i ++){
            if (partialDatum[i] > maxValueX){
                maxValueX = partialDatum[i];
            }
        }
        return maxValueX;
    }

    private static int minValue(int[] partialDatum) { //minValue of array

        int minValueX = partialDatum[0];
        for(int i = 1; i < partialDatum.length; i ++){
            if (partialDatum[i] < minValueX){
                minValueX = partialDatum[i];
            }
        }
        return minValueX;
    }
    
    private int maxRange(Datum[] dataList) {
        int dimensions = dataList[0].x.length;
        int maxRange = minusD(dataList, 0);
        int currentD = 0;

        for (int i = 0; i < dimensions; ++i) {

            if (maxRange < minusD(dataList, i)) {
                currentD = i;
            }
        }
        return currentD;
    }

    private static int splitPoint(int[] partialDatum) { //returns int value of which point in a certain dimension has to be split at

        int bruhhhhhh = (maxValue(partialDatum)+minValue(partialDatum));
        int splitPointBruh = Math.floorDiv(bruhhhhhh, 2);
        return splitPointBruh;
    }

    private static int rangeValue(int[] partialDatum) { //returns range

        int bruhRange = maxValue(partialDatum)-minValue(partialDatum);
        return bruhRange;

    }

    private static int indexOfRange(int[] rangeArray, int index) {
        int rangeIndex=0;
        for (int i = 0; i < rangeArray.length; i ++) {
            if (index == rangeArray[i]) {
                rangeIndex = i + 1; // this is the splitDim
                break;
            }

        }
        return rangeIndex;
    }
    
    private int minusD(Datum[] dataList, int dimension) {
        int[] bounds = minMax(dataList, dimension);
        return Math.abs(bounds[0] - bounds[1]);
    }

  //-------------------------------------------------
    class KDNode {

        boolean leaf;
        Datum leafDatum;           //  only stores Datum if this is a leaf

        //  the next two variables are only defined if node is not a leaf

        int splitDim;      // the dimension we will split on
        int splitValue;    // datum is in low if value in splitDim <= splitValue, and high if value in splitDim > splitValue

        KDNode lowChild, highChild;   //  the low and high child of a particular node (null if leaf)
        //  You may think of them as "left" and "right" instead of "low" and "high", respectively

        KDNode(Datum[] datalist) throws Exception{

            /*
             *  This method takes in an array of Datum and returns
             *  the calling KDNode object as the root of a sub-tree containing
             *  the above fields.
             */

            //   ADD YOUR CODE BELOW HERE

        	
        	
            if (datalist.length == 1 && datalist[0] != null) {

                leaf=true;
                leafDatum=datalist[0];
               // highChild = null;
               // lowChild = null;
                numLeaves++;
            }

            else {
            	
            	
                int arrayOfRanges[] = new int[k];
                int dimensionKArray[] = new int[datalist.length];
                for (int i = 0; i < k; i++) {
                    for (int j = 0; j<datalist.length; j++) {
                        dimensionKArray[j]=datalist[j].x[i];
                    }
                    arrayOfRanges[i] = rangeValue(dimensionKArray);
                }
                int biggestRange = maxValue(arrayOfRanges);

                if (biggestRange == 0) {//all duplicates
                    leaf=true;
                    leafDatum=datalist[0];
                    //highChild = null;
                    //lowChild = null;
                    numLeaves++;
                }   
                else {//not endpoint (leaf) possible yet
                    leaf=false;
                    //splitDim=indexOfRange(arrayOfRanges, biggestRange); //we know what dimension we're splitting in rn
                        
                    splitDim = maxRange(datalist);
                    int[] differenceArray = minMax(datalist, splitDim);
                    
                    
                    int temp1 = (differenceArray[0] + differenceArray[1]);
                    splitValue = temp1/2;
                 
                    if (0!=temp1%2 && 0>temp1/2 && splitValue<=0 ) { //floor it
                    	
                        splitValue--;
                    }
                    
                    //splitDim++;
                       
                    int[] arrayOfCurrentDimension = new int[datalist.length];
                    for (int w = 0; w<datalist.length; w++) {

                        arrayOfCurrentDimension[w]= datalist[w].x[splitDim];//-1
                    }
                    int splitValue = splitPoint(arrayOfCurrentDimension); //we know at which value we separate the values
                    
                    




                    int lowChildSize = 0, highChildSize = 0;

                    int wxx=0, wyy=0;
                    for (int www = 0; www < datalist.length; www++) {

                        if (datalist[www].x[splitDim] <= splitValue) {//-1

                            lowChildSize = lowChildSize +1;
                        }
                        else {

                            highChildSize = highChildSize +1;
                        }
                    }

                    Datum[] lowDatum = new Datum[lowChildSize];
                    Datum[] highDatum = new Datum[highChildSize];

                    for (int www = 0; www < datalist.length; www++) {

                        if (datalist[www].x[splitDim] <= splitValue) {//-1

                            lowDatum[wxx]=datalist[www];

                            wxx++;

                        }
                        else {
                            highDatum[wyy]=datalist[www];

                            wyy++;
                        }
                    }
                    //R*E*C*U*R*S*I*O*N*T*I*M*E
                    lowChild = new KDNode(lowDatum);
                    highChild = new KDNode(highDatum);

                }


            }

            //   ADD YOUR CODE ABOVE HERE

        }

        public Datum nearestPointInNode(Datum queryPoint) {
            Datum nearestPoint, nearestPoint_otherSide;

            
            //   ADD YOUR CODE BELOW HERE
                          
            if (this.leaf)
                return this.leafDatum;
            else {
            	double queryToBoundary = Math.pow(Math.abs(queryPoint.x[this.splitDim]-this.splitValue),2); //-1
           
                if(queryPoint.x[this.splitDim] < this.splitValue) { //-1
                    nearestPoint = this.lowChild.nearestPointInNode(queryPoint);
                    
                    long queryToPoint = distSquared(nearestPoint, queryPoint);

                    if(queryToPoint<queryToBoundary) {

                        return nearestPoint;
                    }
                    else {
                        nearestPoint_otherSide = highChild.nearestPointInNode(queryPoint);
                        if (queryToPoint >= distSquared(nearestPoint_otherSide, queryPoint)) {
                            return nearestPoint_otherSide;
                        }
                        else {
                            return nearestPoint;
                        }
                    }
                }
                else {
                    nearestPoint = this.highChild.nearestPointInNode(queryPoint);
                    //double queryToBoundary = queryPoint.x[this.splitDim-1]-this.splitValue; //didnt do -1
                    long queryToPoint = distSquared(nearestPoint, queryPoint);
                    if(queryToPoint<queryToBoundary) {

                        return nearestPoint;
                    }
                    else {
                        nearestPoint_otherSide = this.lowChild.nearestPointInNode(queryPoint);
                        if (queryToPoint >= distSquared(nearestPoint_otherSide, queryPoint)) {
                            return nearestPoint_otherSide;
                        }
                        else {
                            return nearestPoint;
                        }
                    }

                }

            }

            //   ADD YOUR CODE ABOVE HERE
        }

        // -----------------  KDNode helper methods (might be useful for debugging) -------------------

        public int height() {
            if (this.leaf)
                return 0;
            else {
                return 1 + Math.max( this.lowChild.height(), this.highChild.height());
            }
        }

        public int countNodes() {
            if (this.leaf)
                return 1;
            else
                return 1 + this.lowChild.countNodes() + this.highChild.countNodes();
        }

        /*
         * Returns a 2D array of ints.  The first element is the sum of the depths of leaves
         * of the subtree rooted at this KDNode.   The second element is the number of leaves
         * this subtree.    Hence,  I call the variables  sumDepth_size_*  where sumDepth refers
         * to element 0 and size refers to element 1.
         */

        public int[] sumDepths_numLeaves(){
            int[] sumDepths_numLeaves_low, sumDepths_numLeaves_high;
            int[] return_sumDepths_numLeaves = new int[2];

            /*
             *  The sum of the depths of the leaves is the sum of the depth of the leaves of the subtrees,
             *  plus the number of leaves (size) since each leaf defines a path and the depth of each leaf
             *  is one greater than the depth of each leaf in the subtree.
             */

            if (this.leaf) {  // base case
                return_sumDepths_numLeaves[0] = 0;
                return_sumDepths_numLeaves[1] = 1;
            }
            else {
                sumDepths_numLeaves_low  = this.lowChild.sumDepths_numLeaves();
                sumDepths_numLeaves_high = this.highChild.sumDepths_numLeaves();
                return_sumDepths_numLeaves[0] = sumDepths_numLeaves_low[0] + sumDepths_numLeaves_high[0] + sumDepths_numLeaves_low[1] + sumDepths_numLeaves_high[1];
                return_sumDepths_numLeaves[1] = sumDepths_numLeaves_low[1] + sumDepths_numLeaves_high[1];
            }
            return return_sumDepths_numLeaves;
        }

    }

    public Iterator<Datum> iterator() {
        return new KDTreeIterator();
    }

    private class KDTreeIterator implements Iterator<Datum> {

        //   ADD YOUR CODE BELOW HERE

        KDNode cur = rootNode;
        ArrayList<Datum> nodeList = new ArrayList<Datum>();

        private void returnTree(KDNode cur, ArrayList<Datum> nodeList) {

            if(cur.leaf == true) {
                nodeList.add(cur.leafDatum);
            }
            else {
                returnTree(cur.lowChild, nodeList);
                returnTree(cur.highChild, nodeList);
            }
        }

       KDTreeIterator() {
        	returnTree(cur, nodeList);
       }

        public boolean hasNext() {
          
        	return !nodeList.isEmpty();
        }

        public Datum next() {
            
            if (hasNext() == true) {
            	
                Datum first = nodeList.get(0);
                nodeList.remove(0);
                
                return first;
            }
            else {
            	
            	return null;
            }
          
        }

        //   ADD YOUR CODE ABOVE HERE

    }
    

}

