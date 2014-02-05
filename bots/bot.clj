
(defn parse-change-outer 
  "splits a comma separated string"
  [line]
  (clojure.string/split line #",\s?"))

(defn parse-change-pair 
  "splits a string '8x1' into a pair of integers"
  [line]
  (map #(Integer. %) (clojure.string/split line #"x")))

(defn parse-change 
  "splits '8x1, 9x5, 6x10, 3x25' into ([8 1] [9 5] [6 10] [3 25])"
  [line] 
  (map parse-change-pair
    (parse-change-outer line)))

(defn encode-change-pair
  "encodes a 2 element list as a NxM string"
  [[p1 p2]]
  (format "%dx%d" p1 p2))

(defn encode-change
  "encodes a change list as a string"
  [change]
  (if (= (count change) 1) 
    (format "%s"
      (encode-change-pair (first change)))
    (format "%s, %s"
      (encode-change-pair (first change))
      (encode-change (rest change)))))

(defn change-as-values
  "multiplies the quantities by their values in an"
  [change]
  (map #(apply * %) change))

(defn change-value
  "Calculates the value of a change-pool"
  [change]
  (apply + (change-as-values change)))

(defn change-filter
  "Removes coin denominations that the count is zero for"
  [change]
  (filter #(> (first %) 0) change))

(defn change-from-inner
  "returns the change-pair that can be taken from the arg pair up to value"
  [value [quantity denomination]]
  ; quot for integer division
  [(min (quot value denomination) quantity) denomination])

(defn change-from
  "returns an amount of change to take from 'change' for 'value'"
  [value change]
  (let [innr (change-from-inner value (first change))]
    (if (= (count change) 1) 
      (list innr)
      (concat [innr] (change-from (- value (apply * innr)) (rest change)) ) )))

(defn do-coinfight
  "Do it!"
  [inp]
  
  ; split the first line to extract turn number and player count
  (let [tl (clojure.string/split (first inp) #"\s+") 
    player-count (Integer. (first tl)) 
    turn-number (Integer. (second tl))
    
    ; the remaining lines are the change pools
    change-pools (map #(parse-change %) (rest inp))
    
    ; work out who's turn it is (who *I* am)
    whos-turn (mod turn-number player-count)
  
    ; split the table change, and the player change
    table-change (first change-pools) 
    player-change (rest change-pools)
    
    ; get my change from the player change
    my-change (nth player-change whos-turn)

    ; select coin of lowest value from my pool
    play-value (second (first (change-filter my-change)))
    
    ; select change based on the play value
    take-change (change-from (- play-value 1) table-change)]
    
    ; return out move
    [play-value take-change]))

; creates a variable inp that is a list of lines
(let [inp (apply list (clojure.string/split (slurp *in*) #"\n"))
  move (do-coinfight inp)]
  (println (first move))
  (println (encode-change (second move))))
