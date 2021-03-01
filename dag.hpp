/*
 * Author： Liwei Ni <nlwmode@gmail.com> 
 * Datatime: 2021/02/26
 * This is a test cpp file to test DAG-related operation
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
            Graph(int isDirected = 1, int capacity = DEFAULT_CAPACITY);
            //@TODO 增加一个深拷贝的构造函数
            ~Graph();
            int getNumberOfVertices(){ return _numOfEdges; };
            int getNumberOfEdges(){ return _numOfVertices; };

            /**
             * 图的相关操作
             */
            void creageGraph(int size_vertex , int size_edge);
            void DFS();
            void DFS(int pos);
            void BFS();

            // AOV与AOE网络的相关操作
            bool AOV_TopoSort();
            void AOE_CritivalPath();
            
            /**
             * 顶点的相关操作
             */
            bool insertVertex(const T& val);
            bool removeVertex(const int pos , T val);

            /**
             * 边的相关操作 
             */
            bool insertEdge(const T& v1 , const T& v2 , const E& Aweight);
            bool removeDirecdtedEdge(const T p1 , const T p2);
            bool removeEdge(const T p1 , const T p2);
            E getWeight(const int& v1 , const int& v2);
            Edge<T, E>* getFirstNeighbor(const T pos);
            Edge<T, E>* getNextNeighbor(const T& v1 , const T& v2);

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
        //_visited = nullptr;
    }

    template<class T , class E>
    Graph<T, E>::~Graph()
    {
        _isDirected = 1;    // 默认是有向图
        _numOfVertices = 0;
        _numOfEdges = 0;
        _capacity = 0;
        delete _adjacency_list;
        //delete[] _visited;
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
    bool Graph<T, E>::removeVertex(const int pos , T val)
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
    Edge<T, E>* Graph<T, E>::getNextNeighbor(const T& pos1 ,const T& pos2 )
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
            std::cin >> tmp_val;
            if( !insertVertex(tmp_val) )
                std::cerr << "Error occur when insertVertex : " << tmp_val << std::endl;
        }
        std::cout << "Please input edges:" << std::endl;
        T tmp_val1 , tmp_val2;
        E tmp_weight;
        for(int k = 1 ; k < size_edge ; ++k)
        {
            std::cin >> tmp_val1 >> tmp_val2 >> tmp_weight;
            if( !insertEdge(tmp_val1 , tmp_val2 , tmp_weight) )
                std::cerr << "Error occur when insertEdge at: (" << tmp_val1 << " , " << tmp_val2 << ")" << std::endl;
        }
    }


    /**
     * @brief 以链表形式输出graph
     */    
    template<class T , class E>
    void Graph<T, E>::print()
    {
        std::cout << "This is the information of the graph:" << std::endl;
        for(int i = 0 ; i < _numOfVertices ; ++i)
        {
            std::cout << "Vertex " << i << " : val = " << _adjacency_list[i].vertex_val 
                                   << " , indegree = " << _adjacency_list[i].vertex_indedgree << " --> ";
            Edge<T, E> *p = _adjacency_list[i].vertex_next;
            while(p)
            {
                std::cout  << p->edge_dest_pos << " ( val = " << _adjacency_list[p->edge_dest_pos].vertex_val 
                                            << " , weight = " << p->edge_weight << ")";
                p = p->edge_link;
                if(p)
                    std::cout << " --> " << std::endl;
            }
            std::cout << std::endl;
        }
    }

    /**
     * @brief 深度优先遍历
     */    
    template<class T , class E>
    void Graph<T, E>::DFS()
    {
        for(int i = 0 ; i < _numOfVertices ; ++i)
            _visited[i] = 0;
        for(int i = 0 ; i < _numOfVertices ; ++i)
        {
            // @TODO 这里应该使用 stack 辅助数据结构来DFS
            if(!_visited[i])
                DFS(i);
        }
    }

    /**
     * @brief 
     */    
    template<class T , class E>
    void Graph<T, E>::DFS(int pos)
    {
        _visited[pos] = 1;
        /* do-something  */
        Edge<T, E> *p = getFirstNeighbor(pos);
        while(p)
        {
            int dest = p->edge_dest_pos;
            if(!_visited[dest])
                DFS(dest);
            p = getNextNeighbor(pos , p->edge_dest_pos);
        }
        // 这里应该对_visted复原吧
        // _visited[pos] = 0;
    }

    /**
     * @brief 
     */    
    template<class T , class E>
    void Graph<T, E>::BFS()
    {
        for(int i = 0 ; i < _numOfVertices ; ++i)
            _visited[i] = 0;
        // @TODO 这里应该使用 queue 辅助结构来进行BFS
        int seq[DEFAULT_CAPACITY + 10] ;
        int front = -1 , rear = -1;
        for(int i = 0 ; i < _numOfVertices ; ++i)
        {
            if(!_visited[i])
            {
                _visited[i] = 1;
                /* do-something  */
                ++rear;
                seq[rear] = i;
                while(front != rear)
                {
                    ++front;
                    int tmp_pos = seq[front];
                    Edge<T, E> *p = getFirstNeighbor(tmp_pos);
                    while(p)
                    {
                        int dest = p->edge_dest_pos;
                        if( !_visited[dest] )
                        {
                            _visited[dest] = 1;
                            /* do-something  */
                            ++rear;
                            seq[rear] = dest;
                        }
                        p = getNextNeighbor(tmp_pos , dest);
                    }
                }
            }
        }
    }

    /**
     * @brief AOV: 有向图无回路，也就是 DAG 的拓扑排序
     *        for LUT mapping
     */    
    template<class T , class E> 
    bool Graph<T, E>::AOV_TopoSort()
    {
        int count = 0;
        int tmp_stack[DEFAULT_CAPACITY], top = -1;
        for(int i = 0 ; i < _numOfVertices ; ++i)
        {
            if( _adjacency_list[i].vertex_indedgree == 0 )
                tmp_stack[++top] = i;
        }
        while( top != -1)
        {
            int tmp_pos = tmp_stack[top--];
            /* do-something  */
            ++count;
            Edge<T, E> *p = _adjacency_list[tmp_pos].vertex_next;
            while(p)
            {
                int tmp_dest = p->edge_dest_pos;
                if( _adjacency_list[tmp_dest].vertex_indedgree == 1 )
                    tmp_stack[++top] = tmp_dest;
                p = p->edge_link;
            }
        }
        if(count < _numOfVertices)
        {
            std::cerr << "RESULT: fail, there are some circles in this graph!" << std::endl;
            return false;
        }
        else
        {
            std::cout << " RESULT: ok, a good AOV graph!" << std::endl;
            return true;
        }
            
    }
    /**
     * @brief AOE网络及关键路径算法（AOE为带权值的AOV网络）
     *        从起点到终点具有最大路径长度的路径为关键路径
     *        for CTS
     */
    template <class T, class E>
    void Graph<T, E>::AOE_CritivalPath()
    {
        int *ve = new int[DEFAULT_CAPACITY] , *vl = new int[DEFAULT_CAPACITY];
        for(int i = 0 ; i < _numOfVertices ; ++i)   ve[i] = 0;
        
        for(int i = 0 ; i < _numOfVertices ; ++i)
        {
            Edge<T, E> *p = _adjacency_list[i].vertex_next;
            while(p)
            {
                int tmp_dest = p->edge_dest_pos;
                if(ve[i] + p->edge_weight > ve[tmp_dest])
                    ve[tmp_dest] = ve[i] + p->edge_weight;
                p = p->edge_link;
            }
        }
        
        // output the result
        std::cout << "ve:" << std::endl;
        for(int i = 0 ; i < _numOfVertices ; ++i)
            std::cout << ve[i] << std::endl;
        for(int i = 0 ; i < _numOfVertices ; ++i)
            vl[i] = ve[i];
        for(int i = _numOfVertices - 1 ; i > -1 ; --i)
        {
            E tmp_gap = 0;
            Edge<T, E> *p = _adjacency_list[i].vertex_next;
            if(p)
                tmp_gap = vl[p->dest] - p->cost;
            while (p)
            {
                int tmp_dest = p->dest;
                if( !(tmp_gap < (vl[tmp_dest] - p->edge_weight)) )
                {
                    tmp_gap = vl[tmp_dest] - p->edge_weight;
                    vl[i] = tmp_gap;
                }
                p = p->edge_link;
            }
        }

        std::cout << "vl:" << std::endl;
        for(int i = 0 ; i < _numOfVertices ; ++i)
            std::cout << vl[i] << std::endl;
        for(int i = 0 ; i < _numOfVertices ; ++i)
        {
            Edge<T, E> *p = _adjacency_list[i].vertex_next;
            while (p)
            {
                int tmp_dest = p->dest;
                int e = ve[i] , l = vl[tmp_dest] - p->edge_weight;
                if( e == l)
                    std::cout << "<" << _adjacency_list[i].vertax_val << "," << _adjacency_list[tmp_dest].vertax_val << "> a critical path step" << std::endl;
            }
            p = p->edge_link;
        }
    }

};
