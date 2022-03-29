#include <vector>

template<class T>
class Set{
    struct Node;

public:
    Set() {
        root = nullptr;
        sz = 0;
    }

    template<typename Iterator>
    Set(Iterator first, Iterator last) {
        root = nullptr;
        sz = 0;
        for (Iterator it = first; it != last; ++it) {
            insert(*it);
        }
    }

    Set(const std::initializer_list<T>& elems) {
        root = nullptr;
        sz = 0;
        for (auto &element: elems) {
            insert(element);
        }
        sz = elems.size();
    }

    Set(const Set& other) {
        root = nullptr;
        sz = 0;
        std::vector < T > keys;
        get_keys(other.root, keys);
        for (auto& key : keys) {
            insert(key);
        }
    }

    ~Set() {
        clear(root);
    }

    Set& operator=(const Set& other) {
        if (this == &other) {
            return (*this);
        }
        clear(root);
        std::vector < T > keys;
        get_keys(other.root, keys);
        for (auto& key : keys) {
            insert(key);
        }
        return (*this);
    }

    class Iterator{
    public:
        Iterator() {
            set = nullptr;
            pointer = nullptr;
        }
        Iterator(const Set* set_, Node* pointer_): set(set_), pointer(pointer_) {};
        Iterator(const Iterator& other): set(other.set), pointer(other.pointer) {};

        void operator=(const Iterator& other) {
            set = other.set;
            pointer = other.pointer;
        }

        Iterator operator++() {
            if (pointer->right != nullptr) {
                pointer = pointer->right;
                while (pointer->left != nullptr) {
                    pointer = pointer->left;
                }
                return *this;
            }

            auto key = pointer->key;

            while (pointer == (pointer->mother)->right && pointer != set->root) {
                pointer = pointer->mother;
            }
            if (pointer == set->root && !(key < set->root->key)) {
                pointer = nullptr;
                return *this;
            }
            pointer = pointer->mother;
            return *this;
        }

        Iterator operator++(int) {
            auto result = *this;
            ++(*this);
            return result;
        }

        Iterator operator--() {
            if (pointer == nullptr) {
                pointer = set->root;
                while (pointer->right != nullptr) {
                    pointer = pointer->right;
                }
                return *this;
            }

            if (pointer->left != nullptr) {
                pointer = pointer->left;
                while (pointer->right != nullptr) {
                    pointer = pointer->right;
                }
                return *this;
            }

            while (pointer == (pointer->mother)->left && pointer != set->root) {
                pointer = pointer->mother;
            }
            pointer = pointer->mother;
            return *this;
        }

        Iterator operator--(int) {
            auto result = *this;
            --(*this);
            return result;
        }

        bool operator==(const Iterator& other) const {
            return (set == other.set) && (pointer == other.pointer);
        }

        bool operator!=(const Iterator& other) const {
            return !(*this == other);
        }

        const T& operator*() const {
            return (pointer->key);
        }

        const T* operator->() const {
            return &(pointer->key);
        }

    private:
        const Set* set;
        Node* pointer;
    };

    const bool operator==(const Set& other) const {
        return (root == other.root) && (sz == other.sz);
    }

    Iterator begin() const {
        if (root == nullptr) {
            return end();
        }
        Node* now = root;
        while (now->left != nullptr) {
            now = now->left;
        }
        return Iterator(this, now);
    }

    Iterator end() const {
        return Iterator(this, nullptr);
    }

    void insert(T key) {
        root = insert(root, key);
        black_color(root);
    }

    void erase(T key) {
        root = erase(root, key);
        black_color(root);
    }

    T min() const {
        return get_min(root);
    }

    std::size_t size() const {
        return sz;
    }

    bool empty() const {
        return (sz == 0);
    }

    Iterator find(T key) const {
        Node* vertex = find(root, key);
        return Iterator(this, vertex);
    }

    Iterator lower_bound(T key) const {
        Node* vertex = lower_bound(root, key);
        return Iterator(this, vertex);
    }

private:
    enum COLORS{Red = 0, Black = 1};

   struct Node{
       T key;
       Node* left;
       Node* right;
       COLORS color;
       Node* mother;

        Node() {
            left = nullptr;
            right = nullptr;
            color = Red;
            key = 0;
            mother = nullptr;
        }

        Node(T key_): key(key_) {
            left = nullptr;
            right = nullptr;
            color = Red;
            mother = this;
        }
   };

    void black_color(Node* vertex) const {
        if (vertex == nullptr) {
            return;
        }
        vertex->color = Black;
    }

    COLORS get_color(Node* vertex) const {
        if (vertex == nullptr) {
            return Black;
        }
        return (vertex->color);
    }

    Node* left_rotate(Node* vertex) {
        if (vertex == nullptr || (vertex->right) == nullptr) {
            return vertex;
        }
        Node* right_child = vertex->right;
        vertex->right = right_child->left;
        right_child->left = vertex;
        right_child->color = vertex->color;
        vertex->color = Red;
        if (vertex->mother == vertex) {
            right_child->mother = right_child;
        } else {
            right_child->mother = vertex->mother;
        }
        vertex->mother = right_child;
        upd_mother(vertex);
        upd_mother(right_child);
        return right_child;
    }

    Node* right_rotate(Node* vertex) {
        if (vertex == nullptr || (vertex->left) == nullptr) {
            return vertex;
        }
        Node* left_child = vertex->left;
        vertex->left = left_child->right;
        left_child->right = vertex;
        left_child->color = vertex->color;
        vertex->color = Red;
        if (vertex->mother == vertex) {
            left_child->mother = left_child;
        } else {
            left_child->mother = vertex->mother;
        }
        vertex->mother = left_child;
        upd_mother(vertex);
        upd_mother(left_child);
        return left_child;
    }

    void change_color(Node* vertex) {
        if (vertex == nullptr) {
            return;
        }
        if (vertex->color == Red) {
            vertex->color = Black;
            return;
        }
        vertex->color = Red;
        return;
    }

    void flip(Node* vertex) {
        if (vertex == nullptr) {
            return;
        }
        change_color(vertex);
        change_color(vertex->left);
        change_color(vertex->right);
    }

    void upd_mother(Node* vertex) {
        if (vertex == nullptr) {
            return;
        }
        if (vertex->left != nullptr) {
            vertex->left->mother = vertex;
        }
        if (vertex->right != nullptr) {
            vertex->right->mother = vertex;
        }
    }

    bool is_equal_keys(const T& first, const T& second) const {
        return !(first < second) && !(second < first);
    }

    Node* insert(Node* vertex, T key) {
        if (vertex == nullptr) {
            sz += 1;
            return new Node(key);
        }

        if (is_equal_keys(vertex->key, key)) {
            return vertex;
        }
        if (key < vertex->key) {
            vertex->left = insert(vertex->left, key);
        } else {
            vertex->right = insert(vertex->right, key);
        }

        if (get_color(vertex->left) == Black && get_color(vertex->right) == Red) {
            vertex = left_rotate(vertex);
        }
        if (vertex->left != nullptr && get_color(vertex->left) == Red && get_color(vertex->left->left) == Red) {
            vertex = right_rotate(vertex);
        }
        if (get_color(vertex->left) == Red && get_color(vertex->right) == Red) {
            flip(vertex);
        }
        upd_mother(vertex);
        return vertex;
    }

    Node* left_move(Node* vertex) {
        flip(vertex);
        if (vertex->right == nullptr || get_color(vertex->right->left) == Black) {
            return vertex;
        }
        vertex->right = right_rotate(vertex->right);
        vertex = left_rotate(vertex);
        flip(vertex);
        return vertex;
    }

    Node* right_move(Node* vertex) {
        flip(vertex);
        if (vertex->left == nullptr || get_color(vertex->left->left) == Black) {
            return vertex;
        }
        vertex = right_rotate(vertex);
        flip(vertex);
        return vertex;
    }

    Node* relax(Node* vertex) {
        if (get_color(vertex->left) == Black && get_color(vertex->right) == Red) {
            vertex = left_rotate(vertex);
        }
        if (vertex->left != nullptr && get_color(vertex->left) == Red && get_color(vertex->left->left) == Red) {
            vertex = right_rotate(vertex);
        }
        if (get_color(vertex->left) == Red && get_color(vertex->right) == Red) {
            flip(vertex);
        }
        return vertex;
    }

    T get_min(Node* vertex) {
        if (vertex->left == nullptr) {
            return vertex->key;
        }
        return get_min(vertex->left);
    }

    Node* erase_min(Node* vertex) {
        if (vertex->left == nullptr) {
            sz -= 1;
            delete vertex;
            return nullptr;
        }
        if (get_color(vertex->left) == Black && get_color(vertex->left->left) == Black) {
            vertex = left_move(vertex);
        }
        vertex->left = erase_min(vertex->left);
        vertex = relax(vertex);
        return vertex;
    }

    Node* erase(Node* vertex, T key) {
        if (vertex == nullptr) {
            return nullptr;
        }

        if (key < vertex->key) {
            if (vertex->left != nullptr && get_color(vertex->left) == Black && get_color(vertex->left->left) == Black) {
                vertex = left_move(vertex);
            }
            vertex->left = erase(vertex->left, key);
            vertex = relax(vertex);
            return vertex;
        }

        if (get_color(vertex->left) == Red) {
            vertex = right_rotate(vertex);
        }
        if (is_equal_keys(key, vertex->key) && vertex->right == nullptr) {
            delete vertex;
            sz -= 1;
            return nullptr;
        }
        if (vertex->right != nullptr && get_color(vertex->right) == Black && get_color(vertex->right->left) == Black) {
            vertex = right_move(vertex);
        }
        if (is_equal_keys(key, vertex->key)) {
            vertex->key = get_min(vertex->right);
            vertex->right = erase_min(vertex->right);
        } else {
            vertex->right = erase(vertex->right, key);
        }
        vertex = relax(vertex);
        return vertex;
    }

    void get_keys(Node* vertex, std::vector < T >& keys) const {
        if (vertex == nullptr) {
            return;
        }
        get_keys(vertex->left, keys);
        keys.emplace_back(vertex->key);
        get_keys(vertex->right, keys);
    }

    void get_nodes(Node* vertex, std::vector < Node* >& nodes) const {
        if (vertex == nullptr) {
            return;
        }
        get_nodes(vertex->left, nodes);
        nodes.emplace_back(vertex);
        get_nodes(vertex->right, nodes);
    }

    void clear(Node* vertex) {
        std::vector < Node* > nodes;
        get_nodes(vertex, nodes);
        for (auto& node : nodes) {
            delete node;
        }
        sz = 0;
        root =  nullptr;
    }

    Node* find(Node* vertex, T key) const {
        if (vertex == nullptr) {
            return nullptr;
        }
        if (is_equal_keys(key, vertex->key)) {
            return vertex;
        }
        if (key < vertex->key) {
            return find(vertex->left, key);
        }
        return find(vertex->right, key);
    }

    Node* lower_bound(Node* vertex, T key) const {
        if (vertex == nullptr) {
            return nullptr;
        }
        if (is_equal_keys(key, vertex->key)) {
            return vertex;
        }
        if (key < vertex->key) {
            Node* tmp = lower_bound(vertex->left, key);
            if (tmp == nullptr) {
                return vertex;
            }
            return tmp;
        }
        return lower_bound(vertex->right, key);
    }

    Node* root;
    std::size_t sz;
};
