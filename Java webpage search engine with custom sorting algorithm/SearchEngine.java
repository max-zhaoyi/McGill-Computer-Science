import java.util.HashMap;
import java.util.ArrayList;

public class SearchEngine {
	public HashMap<String, ArrayList<String> > wordIndex;   // this will contain a set of pairs (String, LinkedList of Strings)	
	public MyWebGraph internet;
	public XmlParser parser;

	public SearchEngine(String filename) throws Exception{
		this.wordIndex = new HashMap<String, ArrayList<String>>();
		this.internet = new MyWebGraph();
		this.parser = new XmlParser(filename);
	}
	
	/* 
	 * This does a graph traversal of the web, starting at the given url.
	 * For each new page seen, it updates the wordIndex, the web graph,
	 * and the set of visited vertices.
	 * 
	 * 	This method will fit in about 30-50 lines (or less)
	 */
	public void crawlAndIndex(String url) throws Exception {
		// TODO : Add code here
		
		internet.addVertex(url);
		internet.setVisited(url, true); //addVertex goes first because cannot setVisited on url without a vertex duh

		ArrayList<String> linkArray = parser.getLinks(url); //parse all links into 'linkArray'
		ArrayList<String> wordArray = parser.getContent(url);
		
		for (String s: wordArray) {
			s=s.toLowerCase();
			
			if (wordIndex.containsKey(s)) {
				if(!wordIndex.get(s).contains(url)) {
				wordIndex.get(s).add(url);	
				}
				
			}
			else {
				ArrayList<String> thisUrl = new ArrayList<String>();
				thisUrl.add(url);
				wordIndex.put(s, thisUrl);
			}
		}
		
		for (String w: linkArray) {
				internet.addVertex(w);
				internet.addEdge(url, w);//addVertex goes first because cannot addEdge on url without a vertex duh
				
			if (!internet.getVisited(w)) {
				crawlAndIndex(w); //recursively crawlAndIndex on all non-visited url links
			}
		}	
	}
	/* 
	 * This computes the pageRanks for every vertex in the web graph.
	 * It will only be called after the graph has been constructed using
	 * crawlAndIndex(). 
	 * To implement this method, refer to the algorithm described in the 
	 * assignment pdf. 
	 * 
	 * This method will probably fit in about 30 lines.
	 */
	public void assignPageRanks(double epsilon) {
		// TODO : Add code here
		
	for (String s: internet.getVertices()) { //set all page ranks to 1
			internet.setPageRank(s, 1);		
		}		
		ArrayList<Double> before = new ArrayList<Double>(); //before contains all ranks =1 
		for (String w: internet.getVertices()) {
			before.add(internet.getPageRank(w));
		}		
		ArrayList<Double> after = computeRanks(internet.getVertices()); //initial computation of ranks (run n1)		
		boolean convergence = false;
		
convergence_outer_loop: 		
		while ((convergence == false)==true) {
			
convergence_inner_loop:	
			for (int i2=0; i2< internet.getVertices().size(); i2++) {		//checks if convergent for ALL i
				if(Math.abs( before.get(i2) - after.get(i2)) < epsilon) {
					convergence = true;					
				}
				else {
					convergence = false;
					break convergence_inner_loop;
				}
			}
			if (convergence ==true) {
				break convergence_outer_loop;
			}//if check didnt break outerloop, proceed:
			before = after;
			int i45 = 0;
			for (String s: internet.getVertices()) { //update all vertices
				internet.setPageRank(s, after.get(i45));	
				i45++;
			}
			after = computeRanks(internet.getVertices());			
		}
		
		int zz= 0;
		for (String s: internet.getVertices()) { //update all vertices
			internet.setPageRank(s, after.get(zz));	
			zz++;
		}
	}

	/*
	 * The method takes as input an ArrayList<String> representing the urls in the web graph 
	 * and returns an ArrayList<double> representing the newly computed ranks for those urls. 
	 * Note that the double in the output list is matched to the url in the input list using 
	 * their position in the list.
	 */
	public ArrayList<Double> computeRanks(ArrayList<String> vertices) {
		// TODO : Add code here
		//double pageRank = 1;
		ArrayList<Double> computed = new ArrayList<Double>();
		for (String s: vertices) { //each string s represents an URL in a vertex
			double pageRank=0.5;
			for (String w : internet.getEdgesInto(s)) { // each string w represents an URL edge
			
				pageRank = pageRank + 0.5*(internet.getPageRank(w)/internet.getOutDegree(w) );
			}
			
			computed.add(pageRank);
		}
		
		return computed;
	}

	
	/* Returns a list of urls containing the query, ordered by rank
	 * Returns an empty list if no web site contains the query.
	 * 
	 * This method should take about 25 lines of code.
	 */
	
	public ArrayList<String> getResults(String query) {
		// TODO: Add code here	
//		HashMap<String, Double> resultHashMap = new HashMap<String, Double>();
//		
//		ArrayList<String> resultArrayList = wordIndex.get(query);
//	
//		for(String s : resultArrayList) { //String s is the key
//				
//			resultHashMap.put(s, internet.getPageRank(s));
//				
//		}
//		
//		if (!resultHashMap.keySet().isEmpty()) {
//			return Sorting.fastSort(resultHashMap);
//		}
//		else {
//			ArrayList<String> emptyString = new ArrayList<String>();
//			return emptyString;
//		}			
//		
		query=query.toLowerCase();
		ArrayList<String> toSort = new ArrayList<String>();

		for(String url: internet.getVertices()){
			ArrayList<String> toSearch = parser.getContent(url);
			if (toSearch.indexOf(query) != -1) toSort.add(url);
		}
		HashMap<String, Double>  ranking = new HashMap<String, Double>();
		for (String url: toSort) {
			ranking.put(url, internet.getPageRank(url));
		}
		ArrayList<String> sorted = Sorting.fastSort(ranking);

		return sorted;
		
		
	}
		
		
		
}








