# Airport_GraphAlgorithm
 COP3530 Final: Software that consumes airport data and translate that into a graph of fligths around the USA. 
 Vertices are airports and edges are direct flights between two airports.

 ![alt text](https://github.com/corygrossman/Airport_GraphAlgorithm/blob/main/data/airport_frontend.PNG?raw=true)

 ## Software Logic
A program that ingests airline data (3 million flights 1990-2008) and creates a map of the united states through using a graph data structure.
The program can then animate all of the airports (red nodes) that have a direct flight from the searched airport (blue node) using the depth first or bredth first search algorithm.

![alt text](https://github.com/corygrossman/Airport_GraphAlgorithm/blob/main/data/airport_dfs.PNG?raw=true)

 ## Languages
 - C++
 - QT GUI

 ## Code Worked on:
  - QT GUI
  - Graph and Node data structures
  - Converting airport longitude and latitude values to display correctly over a map of the united states
  - Adding ingested data into nodes and forming relationships with other nodes via a graph structure

## Example Code Screenshots:

### GUI:
  ![alt text](https://github.com/corygrossman/Airport_GraphAlgorithm/blob/main/data/frontend_code.PNG?raw=true)

### Graph Structure:
  ![alt text](https://github.com/corygrossman/Airport_GraphAlgorithm/blob/main/data/graph_structure.PNG?raw=true)

### Node Structure:
  ![alt text](https://github.com/corygrossman/Airport_GraphAlgorithm/blob/main/data/node_structure.PNG?raw=true)

### Creation of the graph based on frontend button press:
  ![alt text](https://github.com/corygrossman/Airport_GraphAlgorithm/blob/main/data/graph_code.PNG?raw=true)