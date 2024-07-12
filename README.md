1. 簡介
   
解決旅行地圖問題，我們首先使用暴力搜索計算最近的兩個POIs及其距離，然後，計算POIs的凸包和面積，同樣使用暴力搜索。

最具挑戰性的是解決旅行商問題(TSP)，我們使用暴力搜索和窮舉搜索，時間複雜度取決於POI數量。

另外，我們提出Convex-Hull-TSP演算法，時間複雜度取決於POI數量，並計算總距離。

詳細分析和研究可確定演算法效率。


2. 問題

  TSP(n)問題定義：走完n個點的最短路徑。
  求TSP (n)，實作與分析
  
    1.	哪兩個 POIs 靠最近? 距離多少? (brute-force algorithm) 
    2.	這些POIs的範圍有多大 (Convex-Hull，它的面積以及最遠的距離)? (brute-force algorithm)
    3.	
      (a)	假如有一個遊客(目前在某一個 POI a)要到所有 n 個 POIs，再回到原來的 POI a，則 最短行程距離是多少(最佳解)? 時間複雜度? (TSP problem: exhaustive search + brute-force algorithm)
      (b)	假如我們設計一個新演算法：(Convex-Hull-TSP Algorithm)
        i.	  求出所有點的 Convex-Hull 
        ii.	  除 Convex-Hull 上的點外，其餘 POIs 找出離 Convex-Hull 邊最近的點投影 
        iii.	按照投影點依序由 Extreme Point 點開始旅遊，再繞回起始點 請問該演算法的時間複雜度(以 big O 表示)以及(行程)花多少距離?
 
3. 理論分析
   
        1.	Closest pairs理論
          用於找出哪兩個POIs最靠近以及它們之間的距離的演算法屬於暴力搜索 (Brute-Force Algorithm)。這是一種簡單但效率較低的方法，它需要計算每一對POIs之間的距離並找出最短的距離，利用Adjacency Matrix儲存所有各點間的距離。
        2.	Convex-hull理論
          計算POIs的凸包和凸包的面積，同樣使用暴力搜索 (Brute-Force Algorithm)。這涉及尋找凸包的邊界點，然後計算凸包的面積，以及最遠點之間的距離。
        3.	TSP理論
          這是一種簡單但計算量巨大的方法，通過列舉所有可能的路徑來找到最短路徑。
        4.	Convex-hull-TSP理論
         
          i.   求出所有點的凸包。
          ii.  找出凸包邊界以外的POIs中，距離凸包邊最近的點的投影點。
          iii. 按照投影點的順序來規劃旅行路徑，然後回到起點。
