#include <cassert>
#include <vector>

template<class T>
class Set{
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

    class iterator{
    public:
        iterator() {
            set = nullptr;
            pointer = nullptr;
        }
        iterator(const Set* set_, Node* pointer_): set(set_), pointer(pointer_) {};
        iterator(const iterator& other): set(other.set), pointer(other.pointer) {};

        void operator=(const iterator& other) {
            set = other.set;
            pointer = other.pointer;
        }

        iterator operator++() {
            assert(pointer != nullptr);

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

        iterator operator++(int) {
            auto result = *this;
            ++(*this);
            return result;
        }

        iterator operator--() {

            if (pointer == nullptr) {
                pointer = set->root;
                while (pointer->right != nullptr) {
                    pointer = pointer->right;
                }
                return *this;
            }

            assert(*this != set->begin());

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

        iterator operator--(int) {
            auto result = *this;
            --(*this);
            return result;
        }

        bool operator==(const iterator& other) const {
            return (set == other.set) && (pointer == other.pointer);
        }

        bool operator!=(const iterator& other) const {
            return !(*this == other);
        }

        const T& operator*() const {
            assert(pointer != nullptr);
            return (pointer->key);
        }

        const T* operator->() const {
            assert(pointer != nullptr);
            return &(pointer->key);
        }

    private:
        const Set* set;
        Node* pointer;
    };

    const bool operator==(const Set& other) const {
        return (root == other.root) && (sz == other.sz);
    }

    iterator begin() const {
        if (root == nullptr) {
            return end();
        }
        Node* now = root;
        while (now->left != nullptr) {
            now = now->left;
        }
        return iterator(this, now);
    }

    iterator end() const {
        return iterator(this, nullptr);
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

    iterator find(T key) const {
        Node* v = find(root, key);
        return iterator(this, v);
    }

    iterator lower_bound(T key) const {
        Node* v = lower_bound(root, key);
        return iterator(this, v);
    }

private:
    void black_color(Node* v) const {
        if (v == nullptr) {
            return;
        }
        v->color = Black;
    }

    COLORS get_color(Node* v) const {
        if (v == nullptr) {
            return Black;
        }
        return (v->color);
    }

    Node* left_rotate(Node* v) {
        if (v == nullptr || (v->right) == nullptr) {
            return v;
        }
        Node* t = v->right;
        v->right = t->left;
        t->left = v;
        t->color = v->color;
        v->color = Red;
        if (v->mother == v) {
            t->mother = t;
        } else {
            t->mother = v->mother;
        }
        v->mother = t;
        upd_mother(v);
        upd_mother(t);
        return t;
    }

    Node* right_rotate(Node* v) {
        if (v == nullptr || (v->left) == nullptr) {
            return v;
        }
        Node* t = v->left;
        v->left = t->right;
        t->right = v;
        t->color = v->color;
        v->color = Red;
        if (v->mother == v) {
            t->mother = t;
        } else {
            t->mother = v->mother;
        }
        v->mother = t;
        upd_mother(v);
        upd_mother(t);
        return t;
    }

    void change_color(Node* v) {
        if (v == nullptr) {
            return;
        }
        if (v->color == Red) {
            v->color = Black;
            return;
        }
        v->color = Red;
        return;
    }

    void flip(Node* v) {
        if (v == nullptr) {
            return;
        }
        change_color(v);
        change_color(v->left);
        change_color(v->right);
    }

    void upd_mother(Node* v) {
        if (v == nullptr) {
            return;
        }
        if (v->left != nullptr) {
            v->left->mother = v;
        }
        if (v->right != nullptr) {
            v->right->mother = v;
        }
    }

    bool is_equal_keys(const T& a, const T& b) const {
        return !(a < b) && !(b < a);
    }

    Node* insert(Node* v, T key) {
        if (v == nullptr) {
            sz += 1;
            return new Node(key);
        }

        if (is_equal_keys(v->key, key)) {
            return v;
        }
        if (key < v->key) {
            v->left = insert(v->left, key);
        } else {
            v->right = insert(v->right, key);
        }

        if (get_color(v->left) == Black && get_color(v->right) == Red) {
            v = left_rotate(v);
        }
        if (v->left != nullptr && get_color(v->left) == Red && get_color(v->left->left) == Red) {
            v = right_rotate(v);
        }
        if (get_color(v->left) == Red && get_color(v->right) == Red) {
            flip(v);
        }
        upd_mother(v);
        return v;
    }

    Node* left_move(Node* v) {
        flip(v);
        if (v->right == nullptr || get_color(v->right->left) == Black) {
            return v;
        }
        v->right = right_rotate(v->right);
        v = left_rotate(v);
        flip(v);
        return v;
    }

    Node* right_move(Node* v) {
        flip(v);
        if (v->left == nullptr || get_color(v->left->left) == Black) {
            return v;
        }
        v = right_rotate(v);
        flip(v);
        return v;
    }

    Node* relax(Node* v) {
        if (get_color(v->left) == Black && get_color(v->right) == Red) {
            v = left_rotate(v);
        }
        if (v->left != nullptr && get_color(v->left) == Red && get_color(v->left->left) == Red) {
            v = right_rotate(v);
        }
        if (get_color(v->left) == Red && get_color(v->right) == Red) {
            flip(v);
        }
        return v;
    }

    T get_min(Node* v) {
        assert(v != nullptr);
        if (v->left == nullptr) {
            return v->key;
        }
        return get_min(v->left);
    }

    Node* erase_min(Node* v) {
        if (v->left == nullptr) {
            sz -= 1;
            delete v;
            return nullptr;
        }
        if (get_color(v->left) == Black && get_color(v->left->left) == Black) {
            v = left_move(v);
        }
        v->left = erase_min(v->left);
        v = relax(v);
        return v;
    }

    Node* erase(Node* v, T key) {
        if (v == nullptr) {
            return nullptr;
        }

        if (key < v->key) {
            if (v->left != nullptr && get_color(v->left) == Black && get_color(v->left->left) == Black) {
                v = left_move(v);
            }
            v->left = erase(v->left, key);
            v = relax(v);
            return v;
        }

        if (get_color(v->left) == Red) {
            v = right_rotate(v);
        }
        if (is_equal_keys(key, v->key) && v->right == nullptr) {
            delete v;
            sz -= 1;
            return nullptr;
        }
        if (v->right != nullptr && get_color(v->right) == Black && get_color(v->right->left) == Black) {
            v = right_move(v);
        }
        if (is_equal_keys(key, v->key)) {
            v->key = get_min(v->right);
            v->right = erase_min(v->right);
        } else {
            v->right = erase(v->right, key);
        }
        v = relax(v);
        return v;
    }

    void get_keys(Node* v, std::vector < T >& keys) const {
        if (v == nullptr) {
            return;
        }
        get_keys(v->left, keys);
        keys.emplace_back(v->key);
        get_keys(v->right, keys);
    }

    void get_nodes(Node* v, std::vector < Node* >& nodes) const {
        if (v == nullptr) {
            return;
        }
        get_nodes(v->left, nodes);
        nodes.emplace_back(v);
        get_nodes(v->right, nodes);
    }

    void clear(Node* v) {
        std::vector < Node* > nodes;
        get_nodes(v, nodes);
        for (auto& node : nodes) {
            delete node;
        }
        sz = 0;
        root =  nullptr;
    }

    Node* find(Node* v, T key) const {
        if (v == nullptr) {
            return nullptr;
        }
        if (is_equal_keys(key, v->key)) {
            return v;
        }
        if (key < v->key) {
            return find(v->left, key);
        }
        return find(v->right, key);
    }

    Node* lower_bound(Node* v, T key) const {
        if (v == nullptr) {
            return nullptr;
        }
        if (is_equal_keys(key, v->key)) {
            return v;
        }
        if (key < v->key) {
            Node* t = lower_bound(v->left, key);
            if (t == nullptr) {
                return v;
            }
            return t;
        }
        return lower_bound(v->right, key);
    }

    Node* root;
    std::size_t sz;
};
