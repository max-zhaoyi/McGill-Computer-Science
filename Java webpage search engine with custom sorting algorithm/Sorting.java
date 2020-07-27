import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;
import java.util.Map.Entry; // You may need it to implement fastSort

public class Sorting {

	/*
	 * This method takes as input an HashMap with values that are Comparable. 
	 * It returns an ArrayList containing all the keys from the map, ordered 
	 * in descending order based on the values they mapped to. 
	 * 
	 * The time complexity for this method is O(n^2) as it uses bubble sort, where n is the number 
	 * of pairs in the map. 
	 */
    public static <K, V extends Comparable> ArrayList<K> slowSort (HashMap<K, V> results) {
        ArrayList<K> sortedUrls = new ArrayList<K>();
        sortedUrls.addAll(results.keySet());	//Start with unsorted list of urls

        int N = sortedUrls.size();
        for(int i=0; i<N-1; i++){
			for(int j=0; j<N-i-1; j++){
				if(results.get(sortedUrls.get(j)).compareTo(results.get(sortedUrls.get(j+1))) <0){
					K temp = sortedUrls.get(j);
					sortedUrls.set(j, sortedUrls.get(j+1));
					sortedUrls.set(j+1, temp);					
				}
			}
        }
        return sortedUrls;                    
    }
    
    
	/*
	 * This method takes as input an HashMap with values that are Comparable. 
	 * It returns an ArrayList containing all the keys from the map, ordered 
	 * in descending order based on the values they mapped to. 
	 * 
	 * The time complexity for this method is O(n*log(n)), where n is the number 
	 * of pairs in the map. 
	 */
    public static <K, V extends Comparable> ArrayList<K> fastSort(HashMap<K, V> results) { //my implementation of mergesort
    	// ADD YOUR CODE HERE
    	
    	
    	
    	ArrayList<Entry<K, V>> unsortedUrls = new ArrayList<Entry<K, V>>();
    	unsortedUrls.addAll(results.entrySet());
    	ArrayList<Entry<K,V>> sortedUrls = new ArrayList<Entry<K,V>>();
    	if (results.size()>2005) {
    		sortedUrls = normalMergeSort(unsortedUrls);
    	}
    	else {
    		sortedUrls = myMergeInsertSort(unsortedUrls);
    	}
    	ArrayList<K> sortedArrayList = new ArrayList<K>();
    	
    	for (int j = 0; j<sortedUrls.size();j++) {
    		sortedArrayList.add(sortedUrls.get(j).getKey());
    	}
    	
    	return sortedArrayList;

    }
    
    //helper methods
    public static <K, V extends Comparable> ArrayList<Entry <K,V>> myMergeInsertSort(ArrayList<Entry<K, V>> sortedUrls) {
		
		if (sortedUrls.size() == 0 ) return new ArrayList<Entry<K,V>>();
	   	
    	else if (sortedUrls.size() ==1) {
    		return sortedUrls;
    	}   		
    	else {
    		int mid = (sortedUrls.size()-1)/2;

    		ArrayList<Entry<K,V>> list1 = new ArrayList<Entry<K,V>>();
    		ArrayList<Entry<K,V>> list2 = new ArrayList<Entry<K,V>>();
    		
    	
    		for (int i = 0; i<sortedUrls.size(); i++) {
    			if(i<=mid) {
    				list1.add(sortedUrls.get(i));
    				
    			}
    			else if (i>mid) {
    				list2.add(sortedUrls.get(i));
    			}
    		}
    		
   		if (list1.size() <= 16 && list2.size() <= 16) {
   			list1=myInsertion(list1);
   			list2=myInsertion(list2);
   			
   		}
   		else {
   		list1 = myMergeInsertSort(list1);
   		list2 = myMergeInsertSort(list2);
   		}
  		
   		return myMerge(list1, list2);
    	}
	}
    	public static <K, V extends Comparable> ArrayList<Entry <K,V>> normalMergeSort(ArrayList<Entry<K, V>> sortedUrls) {
    		
    		if (sortedUrls.size() == 0 ) return new ArrayList<Entry<K,V>>();
    	   	
        	else if (sortedUrls.size() ==1) {
        		return sortedUrls;
        	}   		
        	else {
        		int mid = (sortedUrls.size()-1)/2;

        		ArrayList<Entry<K,V>> list1 = new ArrayList<Entry<K,V>>();
        		ArrayList<Entry<K,V>> list2 = new ArrayList<Entry<K,V>>();
        		
        	
        		for (int i = 0; i<sortedUrls.size(); i++) {
        			if(i<=mid) {
        				list1.add(sortedUrls.get(i));
        				
        			}
        			else if (i>mid) {
        				list2.add(sortedUrls.get(i));
        			}
        		}
      	
       		list1 = normalMergeSort(list1);
       		list2 = normalMergeSort(list2);      		
       		return myMerge(list1, list2);
        	}
    	}
    	public static <K,V extends Comparable<V>> ArrayList<Entry<K, V>> myInsertion(ArrayList<Entry<K,V>> listx) {
    		//int largest;
    		for (int xyz = 0; xyz < listx.size(); xyz++) {
    			//largest = 0;
    			Entry<K, V> e = listx.get(xyz);
    			int k = xyz;
    			while (k>0 && e.getValue().compareTo(listx.get(k-1).getValue())>0) {
    				listx.set(k, listx.get(k-1));
    				k--;
    			}
    			listx.set(k, e);
    		}

       			return listx;
       			
       		}

    	public static <K, V extends Comparable<V>> ArrayList<Entry<K,V>> myMerge(ArrayList<Entry<K,V>> list1, ArrayList<Entry<K,V>> list2) {
    		
    		ArrayList<Entry<K,V>> list = new ArrayList<Entry<K,V>>();
    		int indexList1 = 0;
    		int indexList2 = 0;
    		int totalSize = 0;

    				while ((indexList1<list1.size()) && (indexList2<list2.size())) {
    					if (list1.get(indexList1).getValue().compareTo(list2.get(indexList2).getValue())>0) {
    						list.add(list1.get(indexList1));
    						indexList1++;
    						totalSize++;
    						
    					}
    					else {
    						
    						list.add(list2.get(indexList2));
    						indexList2++;
    						totalSize++;
    					}
    				}			
    		while ((indexList1<list1.size())) {
    			list.add(list1.get(indexList1));
    			indexList1++;
    			totalSize++;
    		
    		}
    		while ((indexList2<list2.size())) {
    			list.add(list2.get(indexList2));
    			indexList2++;
    			totalSize++;
    		}
    		
    		return list;
    	}
    	
    	
    	
      	
    	
    	

    	
    	
    	
    	
    	
    	
    	
    	
    	
    	
    	
    	
}