/*
 *  This is a test cpp file to test DAG-related operation
 * 
 * 
 */

#include <iostream>
#include <vector>


namespace nlwmode_Test{

    template<class T , class E> class Graph;

    template<class T , class E>
    struct Edge
    {
        public:
            Edge(){}
            Edge(int dest , E weight , Edge<T , E> *link = nullptr)
                :edge_dest_pos(dest), edge_weight(weight), edge_link(link)
                {}
            friend class Graph<T , E>;

        private:
            int edge_dest_pos;              // 一条边的另一个邻接点
            E edge_weight;              // 边的权重
            Edge<T , E> *edge_link;

    };
    
    template<class T , class E>
    struct Vertex
    {
        public:
            T vertex_val;
            int vertex_indedgree = 0;   // 定点的入度，辅助用以AOV网络
            Edge<T, E> *vertex_next;    // 和该顶点相连的边的指针
    };

    #define DEFAULT_CAPACITY 100
    template<class T , class E>
    class Graph{
        public:
            Graph(int isDirected = 1, int capacity = DEFAULT_CAPACITY){}
            //@TODO 增加一个深拷贝的构造函数
            ~Graph(){}
            int getNumberOfVertices(){ return _numOfEdges; };
            int getNumberOfEdges(){ return _numOfVertices; };

            /**
             * 图的相关操作
             */
            void creageGraph(int size_vertex , int size_edge);
            void DFS();
            void DFS(T v);
            void BFS();

            // AOV与AOE网络的相关操作
            bool AOV_TopoSort();
            void AOE_CritivalPath();
            
            /**
             * 顶点的相关操作
             */
            bool insertVertex(const T& val);
            bool removeVertex(cosnt int pos , T val);

            /**
             * 边的相关操作 
             */
            bool insertEdge(const T& v1 , const T& v2 , const E& Aweight);
            bool removeDirecdtedEdge(const T p1 , const T p2);
            bool removeEdge(const T p1 , const T p2);
            E getWeight(const int& v1 , const int& v2);
            Edge<T, E>* getFirstNeighbor(const T pos);
            Edge<T, E>* getNextNeighbor(const T& v1 , const& v2);

            /**
             * 一些其他的操作
             */
            void print();

        private:
             int findVertex(const T& vertex);

        private:
            short _isDirected;                  // 1:有向图 0:无向图 , 默认是有向图
            long long _numOfVertices;           // 顶点的数量
            long long _numOfEdges;              // 边的数量
            
            int _capacity;                       // 顺序表存储的最大空间
            Vertex<T, E> *_adjacency_list;      // 邻接表表示的图
            int _visited[DEFAULT_CAPACITY];        // 是否被访问的标志
    };

    template<class T , class E>
    Graph<T, E>::Graph(int isDirected , int capacity)
        :_isDirected(isDirected), _capacity(capacity)
    {
        _numOfVertices = 0;
        _numOfEdges = 0;
        _adjacency_list = new Vertex<T, E>[capacity];
        _visited = nullptr;
    }

    template<class T , class E>
    Graph<T, E>::~Graph()
    {
        _isDirected = 1;    // 默认是有向图
        _numOfVertices = 0;
        _numOfEdges = 0;
        _capacity = 0;
        delete _adjacency_list;
        delete[] _visited;
    }

    /**
     * @brief 在图中插入节点
     */
    template<class T , class E>
    bool Graph<T, E>::insertVertex(const T& val)
    {
        bool res = false;
        //@TODO 增加节点，一般应该也会增加相应的边，
        // 或者在实际使用中执行 insertVertex && insertEdge 也可以解决
        if( _numOfVertices < _capacity )
        {
            _adjacency_list[_numOfVertices].vertex_val = val;
            _adjacency_list[_numOfVertices].vertex_next = nullptr;
            _numOfVertices++;
            res = true;
        }
        else res = false;
        return res;
    }

    /**
     * @brief 
     */
    template<class T , class E>
    bool Graph<T, E>::removeVertex(cosnt int pos , T val)
    {
        bool res = true;
        if(pos < 0 || pos >= _capacity ) res = false;
        Edge<T, E> *p  = _adjacency_list[pos].vertex_next;
        Edge<T, E> *q;
        // 不仅要删除 pos 节点， 还要删除以 pos 节点为顶点的所有边
        while(p)
        {
            _adjacency_list[pos].vertex_next = p.edge_link;
            delete p;
            --_numOfEdges;
            p = _adjacency_list[pos].vertex_next;   // 这里有些问题，因为一个顶点可能有很多的边
        }
        // 删除指向该 pos 节点的边
        for(int i = 0 ; i < _numOfVertices ; ++i)
        {
            p = _adjacency_list[pos].vertex_next;
            if(p && p->edge_dest_pos == pos)
            {
                _adjacency_list[i].vertex_next = p->edge_link;
                delete p;
            }
            else if( p )    // 需要进行遍历
            {
                q = p->edge_link;
                while(q && q->edge_dest_pos != pos)
                {
                    p = q;
                    q = q->edge_link;
                }
                if(q)
                {
                    p->edge_link = q->edge_link;
                    delete q;
                }
            }
            if (_isDirected) --_numOfVertices;

        }
        --_numOfVertices;
        if( pos != _numOfVertices)  // 该顶点后面的子节点顶替该节点
        {
            _adjacency_list[pos].vertex_val = _adjacency_list[_numOfVertices].vertex_val;
            _adjacency_list[pos].vertex_next = _adjacency_list[_numOfVertices].vertex_next;
            for(int i = 0 ; i < _numOfVertices ; ++i)
            {
                if( i != pos) p = _adjacency_list[i].vertex_next;
                while(p)
                {
                    if( p->edge_dest_pos == _numOfVertices)
                    {
                        p->edge_dest_pos = pos;
                        break;
                    }
                    else
                    {
                        p = p->edge_link;
                    }
                }
            }
        }
        return res;
    }

    /**
     * @brief 找节点对应的下标，-1代表未找到
     */
    template<class T , class E>
    int Graph<T, E>::findVertex(const T& val)
    {
        bool res = -1;
        for(int i = 0 ; i < _numOfVertices ; ++i)
        {
            if(val == _adjacency_list[i].vertex_val)
                res = i;
        }
        return res;
    }

    /**
     * @brief 在 v1 值的顶点以及 v2 值的顶点之间插入边
     */
    template<class T , class E>
    bool Graph<T, E>::insertEdge(const T& v1 , const T& v2 , const E& weight)
    {
        bool res = true;
        int p1 = findVertex(v1) , p2 = findVertex(v2);
        if(p1 == -1 || p2 == -1)    res = false;
        if(_isDirected == 1)    // 有向图
        {
            _adjacency_list[p1].vertex_next = new Edge<T, E>(p2, weight ,_adjacency_list[p1].vertex_next);
            _adjacency_list[p2].vertex_indedgree++;
        }
        else
        {
            _adjacency_list[p1].vertex_next = new Edge<T, E>(p2, weight ,_adjacency_list[p1].vertex_next);
            _adjacency_list[p2].vertex_indedgree++; 
            _adjacency_list[p2].vertex_next = new Edge<T, E>(p1, weight ,_adjacency_list[p2].vertex_next);
            _adjacency_list[p1].vertex_indedgree++;
        }
        _numOfEdges++;
        return res;
    }

    /**
     * @brief 删除从 p1 指向 p2 的边
     */
    template<class T , class E>
    bool Graph<T, E>::removeDirecdtedEdge(const T p1 , const T p2)
    {
        return true;
    }

    /**
     * @brief 删除无向图的边
     */
    template<class T , class E>
    bool Graph<T, E>::removeEdge(const T p1 , const T p2)
    {
        bool res = true;
        if(p1 < 0 || p1 >= _numOfVertices || p2 < 0 || p2 >= _numOfVertices )   res = false;
        Edge<T, E> *p  = _adjacency_list[p1].vertex_next;
        Edge<T, E> *q;
        if(p->edge_dest_pos == p2)
        {
            _adjacency_list[p1].vertex_next = p->edge_link;
            delete p;
        }
        else
        {
            q = p->edge_link;
            while(q && q->edge_dest_pos != p2)
            {
                p = q;
                q = q->edge_link;
            }
            if(q)
            {
                p->edge_link = q->edge_link;
                delete q;
            }
        }
        --_numOfEdges;
        //无向图中需要删除对称的边
        if(_isDirected == 0)        
        {
            p = _adjacency_list[p2].vertex_next;
            if(p->edge_dest_pos == p1)
            {
                _adjacency_list[p2].vertex_next = p->edge_link;
                delete p;
            }
            else
            {
                q = p->edge_link;
                while(q && q->edge_dest_pos != p1)
                {
                    p = q;
                    q = q->edge_link;
                }
                if(q)
                {
                    p->edge_link = q->edge_link;
                    delete q;
                }
            }
        }
        return res;
    }

    /**
     * @brief 
     */
    template<class T , class E>
    Edge<T, E>* Graph<T, E>::getFirstNeighbor(const T pos)
    {   
        if(pos < 0 || pos >= _numOfVertices) return nullptr;
        return _adjacency_list[pos].vertex_next ? _adjacency_list[pos].vertex_next : nullptr; 

    }
    /**
     * @brief 获取 pos1 顶点的邻接点 pos2 的邻接点
     */
    template<class T , class E>
    Edge<T, E>* Graph<T, E>::getNextNeighbor(const T pos1 ,const T pos2 )
    {
        Edge<T, E> *res = nullptr;
        if(pos1 < 0 || pos1 >= _numOfVertices || pos2 < 0 || pos2 >= _numOfVertices ) res = nullptr;

        Edge<T, E> *p = _adjacency_list[pos1].vertex_next;
        while(p && p->edge_dest_pos != pos2)
            p = p->edge_link;
        if(p && p->edge_link)
            res =  p->edge_link;
        return res;
    
    }

/****************************************************************************/
/*****************          for test                    *********************/
/****************************************************************************/
    /**
     * @brief 获取 pos1 顶点的邻接点 pos2 的邻接点
     */
    template<class T , class E>
    void Graph<T, E>::creageGraph(int size_vertex , int size_edge)
    {
        T tmp_val;
        std::cout << "Please input vertices:" << std::endl;
        for(int i = 0 ; i < size_vertex ; ++i)
        {
            cin >> tmp_val;
            if( !insertVertex(tmp_val) )
                std::cerr << "Error occur when insertVertex : " << tmp_val << endl;
        }
        std::cout << "Please input edges:" << std::endl;
        T tmp_val1 , tmp_val2;
        E tmp_weight;
        for(int k = 1 ; k < size_edge ; ++k)
        {
            cin >> tmp_val1 >> tmp_val2 >> tmp_weight;
            if( !insertEdge(tmp_val1 , tmp_val2 , tmp_weight) )
                std::cerr << "Error occur when insertEdge at: (" << tmp_val1 << " , " << tmp_val2 << ")" << endl;
        }
    }
    

};
