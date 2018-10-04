# apriori
Implemented the Apriori algorithm for frequent itemset mining as a part of Data Mining course. 
The code is exceptionally optimised for large databases using very efficient data structures and implementations. 

To run the code, use the following commands:

```
bash ./compile.sh
bash ./run.sh $input file name(such as retail.dat)$ $support threshold$
```

Note: The code was running on the given database of 40,000 transactions with a support threshold of 1% in nearly 1.5 seconds on a UNIX based 4GB RAM machine, running one core at a time.
