#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPixmap>
#include <QMatrix>

using namespace std;

//gathers the data from the dataset
//returns a pair of month and year
pair<int, int> parseMonthYear(string date) {
    string monthString = date.substr(0, 2);
    if (monthString[1] == '/') {
        monthString = monthString.substr(0, 1);
    }
    int month = 0;
    try {
        month = stoi(monthString);
    }
    catch (...) {
        cout << "invalid month: " << endl;
    }

    stringstream s(date);
    string yearString;
    getline(s, yearString, '/');
    getline(s, yearString, '/');
    getline(s, yearString);
    int year = 0;
    try {
        year = stoi(yearString);
    }
    catch (...) {
        cout << "invalid year " << endl;
    }

    return pair<int, int>(month, year);
}

//initializes and defines a vertex in the graph
//vertices include airport code, city, loingitude, and latitude coordinates
class Node {
    string code = "null", city = "null";
    double longitude = -1, latitude = -1;
public:
    Node() {};

    Node(string _code, string _city, double _long, double _lat) {
        code = _code;
        city = _city;
        longitude = _long;
        latitude = _lat;
    };

    void setCode(string _code) { code = _code; }

    void setCity(string _city) { city = _city; }

    void setLong(double _long) { longitude = _long; }

    void setLat(double _lat) { latitude = _lat; }

    string getCode() { return code; }

    string getCity() { return city; }

    double getLat() { return latitude; }

    double getLong() { return longitude; }
};

//defines a graph class
class Graph {
    // List of all airport nodes
    unordered_map<string, Node *> nodeMap;

    // Adjacency List
    // mainList[Node origin] = { Node destination, int distance }
    unordered_map<Node *, vector<pair<Node*, int>>> mainList;

    string ORIGIN_CODE;
    int MAX_DISTANCE;
    int MONTH;
    int YEAR;

public:
    Graph() {
        ORIGIN_CODE = "null";
        MAX_DISTANCE = -1;
        MONTH = -1;
        YEAR = -1;
    };
    Graph(string code, int distance, int month, int year) {
        ORIGIN_CODE = code;
        MAX_DISTANCE = distance;
        MONTH = month;
        YEAR = year;
    }

    //returns the amount of vertices and edges
    int V();
    int E();
    void insertEdge(Node *from, Node *to, int distance);
    bool loadData(int month, int year, int distance);
    unordered_map<string, Node *>& getNodeMap() {
        return nodeMap;
    }
    unordered_map<Node *, vector<pair<Node*, int>>>& getGraph() {
        return mainList;
    }
    int getNumVertices() {
        return nodeMap.size();
    }
    vector<pair<double,double>> bfs(Node *root);
    vector<pair<double,double>> dfs(Node *root);
    void print();
    map<Node *, pair<int, Node *>> &dijkstras(Node *root, int d);
};


void Graph::insertEdge(Node* from, Node* to, int distance) {
    if (nodeMap.find(from->getCode()) != nodeMap.end())
        from = nodeMap[from->getCode()];
    else
        nodeMap[from->getCode()] = from;
    if (nodeMap.find(to->getCode()) != nodeMap.end())
        to = nodeMap[to->getCode()];
    else
        nodeMap[to->getCode()] = to;

    mainList[from].push_back(make_pair(to, distance));

    if (mainList.find(to) == mainList.end())
        mainList[to] = {}; //mainList[to].push_back(make_pair(from, distance));*// Swapping with this line makes it an undirected graph.
}

void Graph::print() {
    unordered_map<Node*, vector<pair<Node*, int>>>::iterator it;
    for (it = mainList.begin(); it != mainList.end(); ++it)
    {
        cout << it->first->getCode() << "->";
        for (int j = 0; j < it->second.size(); j++)
            cout << " " << it->second[j].first->getCode() << " " << it->second[j].second << " miles |";
        cout << "\n";
    }
}

vector<pair<double,double>> Graph::bfs(Node *root) {
    set<Node *> visited;
    queue<Node *> fifo;

    vector<pair<double,double>> returnVec;

    visited.insert(root);
    fifo.push(root);

    while (!fifo.empty()) {
        Node *currentNode = fifo.front();
        // Draw circle at currentNode's long and lat
        cout << currentNode->getCode() << endl;
        returnVec.push_back(make_pair(currentNode->getLong(), currentNode->getLat()));
        fifo.pop();

        for (pair<Node*, int> v : mainList[currentNode]) {
            if (visited.count(v.first) == 0) {
                visited.insert(v.first);
                fifo.push(v.first);
            }
        }
    }

    return returnVec;
}

vector<pair<double,double>> Graph::dfs(Node *root) {
    set<Node *> visited;
    stack<Node *> lifo;

    vector<pair<double,double>> returnVec;

    visited.insert(root);
    lifo.push(root);

    while (!lifo.empty()) {
        Node *currentNode = lifo.top();
        // Draw circle at currentNode's long and lat
        cout << currentNode->getCode() << endl;
        returnVec.push_back(make_pair(currentNode->getLong(), currentNode->getLat()));

        lifo.pop();

        for (pair<Node*, int> v : mainList[currentNode]) {
            if (visited.count(v.first) == 0) {
                visited.insert(v.first);
                lifo.push(v.first);
            }
        }
    }
    return returnVec;
}

map<Node*, pair<int, Node*>>& Graph::dijkstras(Node *root, int d) {
    // Initialize data containers
    set<Node *> completedSet;
    set<Node *> notCompletedSet;

    // Map key node is equal to int distance and Node* predecessor
    map<Node *, pair<int, Node *>> optimizedChart;

    // Initialize vectors with default values
    for (unordered_map<string, Node*>::iterator iter = nodeMap.begin(); iter != nodeMap.end(); ++iter) {
        notCompletedSet.insert(iter->second);
        optimizedChart[iter->second] = make_pair(10000000, nullptr); // Set to int value larger than any distance
    }

    // Place root node in completed set and remove from not completed set
    optimizedChart[root].first = 0;
    completedSet.insert(root);
    notCompletedSet.erase(root);

    // Iterate through the not completed set and find all connections and update chart
    while (!notCompletedSet.empty()) {
        Node *currentNode = (*notCompletedSet.begin());
        completedSet.insert(currentNode);
        notCompletedSet.erase(currentNode);
        int totalDistance = 0;
        if (optimizedChart[currentNode].first < 10000000) {
            totalDistance = optimizedChart[currentNode].first;
        }

        for (pair<Node*, int> v : mainList[currentNode]) {
            // If distance between currentNode and v + totol distance is less than the value on the chart
            if (v.second + totalDistance < optimizedChart[v.first].first) {
                optimizedChart[v.first].first = v.second + totalDistance;
                totalDistance = optimizedChart[v.first].first;
                optimizedChart[v.first].second = currentNode;
            }
        }
    }

    // Print results
    for (map<Node *, pair<int, Node *>>::iterator iter = optimizedChart.begin(); iter != optimizedChart.end(); ++iter) {
        if(iter->second.first < d){
            cout << "Origin: " << iter->first->getCode() << endl;
            cout << "Distance: " << iter->second.first << endl;
            if (iter->second.second != nullptr) {
                cout << "Predecessor: " << iter->second.second->getCode() << endl << endl;
            } else {
                cout << "Predecessor: -1" << endl << endl;
            }
        }
    }

    cout << "Done" << endl;
    return optimizedChart;
}

//reads the csv data and creates a graph from it
bool Graph::loadData(int month, int year, int distance) {

    MAX_DISTANCE = distance;
    MONTH = month;
    YEAR = year;

    ifstream myfile;
    vector<string> row;
    vector<vector<string>> data;
    myfile.open("C://FTP/Cory/University of Florida/Data Structures and Algorithms/Project3 87/YelpGraphGUI/Airports2.csv");
    //myfile << "Writing this to a file.\n";
    if (!myfile.is_open()) {
        cout << "File Not Found" << endl;
        return false;
    }
    string word;
    int count = 0;
    int lineIndex = 0;
    string line;
    getline(myfile, line, '\n');
    while (myfile) {
        Node* origin = new Node;
        Node* dest = new Node;
        getline(myfile, line, '\n');

        stringstream s(line);
        if (line == "")
            break;

        getline(s, word, ','); //Origin Code
        origin->setCode(word);

        int dist = 0;
        int mm = 0;
        int yy = 2000;
        getline(s, word, ','); //Destination Code
        dest->setCode(word.substr(0, 3));

        getline(s, word, '"');//Origin City
        getline(s, word, '"');
        origin->setCity(word);

        getline(s, word, '"'); // Destination City
        getline(s, word, '"');
        dest->setCity(word);

        getline(s, word, ',');
        getline(s, word, ',');//passwngers

        getline(s, word, ',');//seats

        getline(s, word, ',');

        getline(s, word, ',');
        dist = stoi(word);

        getline(s, word, ',');
        pair<int, int> mmYY = parseMonthYear(word);
        mm = mmYY.first;
        yy = mmYY.second;

        // Org and dest population
        getline(s, word, ',');
        getline(s, word, ',');


        getline(s, word, ',');
        if (word == "NA") {
            origin->setLat(0);
            continue;
        }
        else {
            origin->setLat(stof(word));
        }

        getline(s, word, ',');
        if (word == "NA") {
            origin->setLong(0);
        }
        else {
            origin->setLong(stof(word));
        }

        getline(s, word, ',');
        if (word == "NA") {
            dest->setLat(0);
            continue;
        }
        else {
            dest->setLat(stof(word));
        }

        getline(s, word, ',');
        if (word == "NA") {
            dest->setLong(0);
        }
        else {
            dest->setLong(stof(word));
        }

        //Adjust to do by month
        if (mm == MONTH && yy == YEAR && dist < MAX_DISTANCE && dist > 0) {
            insertEdge(origin, dest, dist);
            if (count == 0)
                ORIGIN_CODE = origin->getCode();
        }
        if (YEAR == yy && MONTH == mm) {
            //cout << "Line:  " << lineIndex << " Count " << ++count << " " << line << endl;
        }
        lineIndex++;
    }

    cout << "Done" << endl;
    //cout << lineIndex << endl;
    myfile.close();
}

//initializes user inputted values in the gui
int user_Dist = 500;
int user_Month = 1;
int user_Year = 1990;
string user_airportCode;
Graph* graph;
vector<pair<double,double>> vectorLongLat;
vector<pair<pair<double,double>,pair<double,double>>> routeLine;

bool runBFS = false;
vector<pair<double,double>> bfsOrder;
int bfsIter = 0;

bool runDFS = false;
vector<pair<double,double>> dfsOrder;
int dfsIter = 0;


//defines and connects the ui elements to the backend
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //whenever the search button is pressed, it calls the SearchPressed() function
    QString search = "searchButton";
    QPushButton *searchButton = MainWindow::findChild<QPushButton*>(search);
    connect(searchButton, SIGNAL(released()),this, SLOT(SearchPressed()));

    //when the bfs button is pressed
    QString bfs = "bfsButton";
    QPushButton *bfsButton = MainWindow::findChild<QPushButton*>(bfs);
    connect(bfsButton, SIGNAL(released()),this, SLOT(BFSPressed()));

    QString dfs = "dfsButton";
    QPushButton *dfsButton = MainWindow::findChild<QPushButton*>(dfs);
    connect(dfsButton, SIGNAL(released()),this, SLOT(DFSPressed()));

    QString dij = "dijkstraButton";
    QPushButton *dijButton = MainWindow::findChild<QPushButton*>(dij);
    connect(dijButton, SIGNAL(released()),this, SLOT(DIJPressed()));


    //adds items into the year comboBox
    int startIndex = 1990;
    for(int i = 0; i < 20; i++){
        ui->yearSelect->addItem(QString::number(startIndex));
        startIndex++;
    }


}

MainWindow::~MainWindow()
{
    delete ui;
}


//maps values from a range to a different range
//used for mapping long and lat values onto the pixel range of the graphic
double mapValues(double oldVal, double oldMin, double oldMax, double newMin, double newMax){
    double oldRange = (oldMax - oldMin);
    double newRange = (newMax - newMin);
    double newValue = (((oldVal - oldMin) * newRange) / oldRange) + newMin;
    return newValue;
}

//returns all of the long and lat values to be looped through and displayed as circles
vector<pair<double,double>> LongLatVals(Graph* g){

    //vector of (long,lat) values to be returned
    vector<pair<double,double>> vecLongLat;

    unordered_map<Node*, vector<pair<Node*, int>>>::iterator it;
    double originLong, originLat, destLong, destLat;
    int dist;


    //loops through all of the nodes in the graph using iterators
    for (it = g->getGraph().begin(); it != g->getGraph().end(); ++it)
    {

        //gets the longitude and latitude values from the origin airport
        originLong = it->first->getLong();
        originLat = it->first->getLat();


             //maps the long and lat values to be useable to dispaly in the window
             originLong = mapValues(originLong, -65,-125, 1350, 0);
             originLat = mapValues(originLat, 25,48, 794, 23.2);

             //makes sure the origin long and lat values are valid
             if(originLong < 1350 && originLong > 0 && originLat < 794 && originLat > 23.2){
                //adds the values into the vector
                pair<double,double> longLat = make_pair(originLong,originLat);
                vecLongLat.push_back(longLat);
             }


            for (unsigned int j = 0; j < it->second.size(); j++)
            {
                //gets the longitude, latitude, adn distance from the origin airport to the destination airport
                destLong = it->second[j].first->getLong();
                destLat = it->second[j].first->getLat();
                dist = it->second[j].second;


              //maps the long and lat values to be useable to dispaly in the window
                destLong = mapValues(destLong, -65,-125, 1350, 0);
                destLat = mapValues(destLat, 25,48, 794, 23.2);

                //makes sure the destination long and lat values are valid
                if(destLong < 1350 && destLong > 0 && destLat < 794 && destLat > 23.2){
                   //adds the values into the vector
                   pair<double,double> longLat = make_pair(destLong,destLat);
                   vecLongLat.push_back(longLat);
                }

           }
    }
    return vecLongLat;
}

//returns all of the routes with their long and lat values
vector<pair<pair<double,double>,pair<double,double>>> GetRoutes(Graph* g){

    //vector of (long,lat) values to be returned
    vector<pair<pair<double,double>,pair<double,double>>> routes;
    pair<double,double> originLongLat;
    pair<double,double> destLongLat;

    unordered_map<Node*, vector<pair<Node*, int>>>::iterator it;
    double originLong, originLat, destLong, destLat;
    int dist;
    bool add;


    //loops through all of the nodes in the graph
    for (it = g->getGraph().begin(); it != g->getGraph().end(); ++it)
    {
        add = true;

        //gets the longitude and latitude values from the origin airport
        originLong = it->first->getLong();
        originLat = it->first->getLat();


             //maps the long and lat values to be useable to dispaly in the window
             originLong = mapValues(originLong, -65,-125, 1350, 0);
             originLat = mapValues(originLat, 25,48, 794, 23.2);

             //makes sure the origin long and lat values are valid
             if(originLong < 1350 && originLong > 0 && originLat < 794 && originLat > 23.2){
                //adds the values into the vector
                originLongLat = make_pair(originLong,originLat);
             }
             else{
                 add = false;
             }



            for (unsigned int j = 0; j < it->second.size(); j++)
            {
                //gets the longitude, latitude, adn distance from the origin airport to the destination airport
                destLong = it->second[j].first->getLong();
                destLat = it->second[j].first->getLat();
                dist = it->second[j].second;


              //maps the long and lat values to be useable to dispaly in the window
                destLong = mapValues(destLong, -65,-125, 1350, 0);
                destLat = mapValues(destLat, 25,48, 794, 23.2);

                //makes sure the destination long and lat values are valid
                if(destLong < 1350 && destLong > 0 && destLat < 794 && destLat > 23.2 && add == true){
                   //adds the values into the vector
                   destLongLat = make_pair(destLong,destLat);
                   //combines routes
                   routes.push_back(make_pair(originLongLat,destLongLat));
                }


           }
    }
    return routes;
}

//this function creates the graph for the given airport code, distance, date range, and passangers
void MainWindow::SearchPressed(){
    runBFS = false;
    runDFS = false;

    //while loop that does not finish until the user has properly inputted a correct code
    if(user_airportCode == ""){
        ui->airportCode->setPlaceholderText("Please Enter a Valid Code:");
    }

    //when searchButton is pressed, it sets all of the user values to the initialized variables
    user_airportCode = ui->airportCode->text().toStdString();
    user_Month = ui->monthSelect->currentText().toInt();
    user_Year = ui->yearSelect->currentText().toInt();
    user_Dist = ui->maxDist->text().toInt();

    //logs the correct usercode into the command line
    cout << user_airportCode << endl;

    // Instantiate Graph object
    graph = new Graph(user_airportCode, 100000, user_Month, user_Year);

    // If user clicks search, Load data from .csv
    graph->loadData(user_Month, user_Year, 100000);


    //gets where all of the nodes are in the graph
    vectorLongLat = LongLatVals(graph);
    //gets where all of the routes are in the graph
    routeLine = GetRoutes(graph);


//    cout << user_Month << endl;
//    cout << user_Year << endl;
//    cout << user_Dist << endl;

    //this could be used to add available origin airports to the UI
    //TODO: add for loop that adds all the available airports in given month and year to a comboBox

}


void MainWindow::BFSPressed(){
    runDFS = false;
    runBFS = true;
    bfsIter = 1;
    Node* root = graph->getNodeMap()[user_airportCode];

    bfsOrder = graph->bfs(root);
    cout << "Finished" << endl;

    double originLong,originLat;

    //maps the long and lat to be the correct nodes
    for(int i = 0; i < bfsOrder.size(); i++){

        originLong = bfsOrder.at(i).first;
        originLat = bfsOrder.at(i).second;
        bfsOrder.at(i).first = mapValues(originLong, -65,-125, 1350, 0);
        bfsOrder.at(i).second = mapValues(originLat, 25,48, 794, 23.2);
        //cout << bfsOrder.at(i).first << ", " << bfsOrder.at(i).second << endl;
    }

}

void MainWindow::DFSPressed(){
    runBFS = false;
    runDFS = true;
    dfsIter = 1;

    Node* root = graph->getNodeMap()[user_airportCode];

    dfsOrder = graph->dfs(root);
    cout << "Finished" << endl;

    double originLong,originLat;

    //maps the long and lat to be the correct nodes
    for(int i = 0; i < dfsOrder.size(); i++){

        originLong = dfsOrder.at(i).first;
        originLat = dfsOrder.at(i).second;
        dfsOrder.at(i).first = mapValues(originLong, -65,-125, 1350, 0);
        dfsOrder.at(i).second = mapValues(originLat, 25,48, 794, 23.2);
        //cout << dfsOrder.at(i).first << ", " << dfsOrder.at(i).second << endl;
    }

}

void MainWindow::DIJPressed(){
    Node* root = graph->getNodeMap()[user_airportCode];
    graph->dijkstras(root, user_Dist);
}


//used for drawing everything that needs to be inputted manualually to the screen
void MainWindow::paintEvent(QPaintEvent *event){


    QPainter painter(this);

    QPen pen;
    QColor col;

    int radius = 2;
    QPoint source;
    source.setX(0);
    source.setY(0);

    QPoint end;
    end.setX(0);
    end.setY(0);

    //label_2 is helful in determing the window to draw in
    int width = ui->label_2->width();
    int height = ui->label_2->height();

    //translates the drawing frame to the center of the display
    QMatrix matrix;
    matrix.translate(ui->label_2->x()+100,ui->label_2->y() + 100);
    painter.setMatrix(matrix);

    QPixmap pm("C:/FTP/Cory/University of Florida/Data Structures and Algorithms/Project3 87/YelpGraphGUI/usamap.png"); // <- path to image file
    painter.drawPixmap(0,0,1260,868, pm);


    //print out the graph when graph is filled with elements
    //cout << vectorLongLat.size() << endl;

    //adjusts the trasnform matrix for where the usa map is dispalyed
    matrix.translate(5,23.2);
    painter.setMatrix(matrix);

    if(vectorLongLat.size() > 0){
        //cout << vectorLongLat.size() << endl;

        //prints all of the routes
        col.setRgb(150,151,135);
        pen.setColor(col);
        pen.setWidth(1);
        painter.setPen(pen);
        for(int i = 0; i < routeLine.size(); i++){
            pair<pair<double,double>,pair<double,double>> temp;
            temp = routeLine.at(i);


            //cout << temp.first.first << ", " << temp.first.second << " -> " << temp.second.first << ", " << temp.second.second << endl;

            //longitude for origin
            source.setX(temp.first.first);
            //latitude for origin
            source.setY(temp.first.second);

            end.setX(temp.second.first);
            end.setY(temp.second.second);

            painter.drawLine(source,end);
        }

        update();

        col.setRgb(254,211,140);
        pen.setColor(col);
        pen.setWidth(3);
        painter.setPen(pen);
        //prints all of the nodes over the routes
        for(int i = 0; i < vectorLongLat.size(); i++){
            pair<double,double> temp;
            temp = vectorLongLat.at(i);

            //longitude
            source.setX(vectorLongLat.at(i).first);
            //latitude
            source.setY(vectorLongLat.at(i).second);
            painter.drawEllipse(source, radius, radius);
        }

        update();

        if(runBFS){
            //draws the root node first
            col.setRgb(23,68,137);
            pen.setColor(col);
            pen.setWidth(7);
            painter.setPen(pen);
            source.setX(bfsOrder.at(0).first);
            source.setY(bfsOrder.at(0).second);
            painter.drawEllipse(source, 5,5);

            col.setRgb(255,0,0);
            pen.setColor(col);
            pen.setWidth(3);
            painter.setPen(pen);
            for(int i = 1; i < bfsIter; i++){
                source.setX(bfsOrder.at(i).first);
                source.setY(bfsOrder.at(i).second);
                painter.drawEllipse(source, radius,radius);
            }
            bfsIter++;

            if(bfsIter >= bfsOrder.size()){
                bfsIter = bfsOrder.size();
            }
        }

        if(runDFS){
            //draws the root node first
            col.setRgb(23,68,137);
            pen.setColor(col);
            pen.setWidth(7);
            painter.setPen(pen);
            source.setX(dfsOrder.at(0).first);
            source.setY(dfsOrder.at(0).second);
            painter.drawEllipse(source, 5,5);

            col.setRgb(255,0,0);
            pen.setColor(col);
            pen.setWidth(3);
            painter.setPen(pen);
            for(int i = 1; i < dfsIter; i++){
                source.setX(dfsOrder.at(i).first);
                source.setY(dfsOrder.at(i).second);
                painter.drawEllipse(source, radius,radius);
            }
            dfsIter++;

            if(dfsIter >= dfsOrder.size()){
                dfsIter = dfsOrder.size();
            }
        }
    }

}



