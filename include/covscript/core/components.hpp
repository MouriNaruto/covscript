#pragma once
/*
* Covariant Script Basic Components
*
* Licensed under the Covariant Innovation General Public License,
* Version 1.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* https://covariant.cn/licenses/LICENSE-1.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
* Copyright (C) 2019 Michael Lee(李登淳)
* Email: mikecovlee@163.com
* Github: https://github.com/mikecovlee
*/
#include <covscript/import/mozart/base.hpp>

namespace cs {
    // Exceptions
    class exception final : public std::exception {
        std::string mWhat;
    public:
        exception() = delete;

        exception(std::size_t line, const std::string &file, const std::string &code, const std::string &what) noexcept:
                mWhat("File \"" + file + "\", line " + std::to_string(line) + "\n\t" + code + "\n\t^\n" + what) {}

        exception(const exception &) = default;

        exception(exception &&) noexcept = default;

        ~exception() override = default;

        exception &operator=(const exception &) = default;

        exception &operator=(exception &&) = default;

        const char *what() const noexcept override
        {
            return this->mWhat.c_str();
        }
    };

    class runtime_error final : public std::exception {
        std::string mWhat = "Runtime Error";
    public:
        runtime_error() = default;

        explicit runtime_error(const std::string &str) noexcept:
                mWhat("Runtime Error: " + str) {}

        runtime_error(const runtime_error &) = default;

        runtime_error(runtime_error &&) noexcept = default;

        ~runtime_error() override = default;

        runtime_error &operator=(const runtime_error &) = default;

        runtime_error &operator=(runtime_error &&) = default;

        const char *what() const noexcept override
        {
            return this->mWhat.c_str();
        }
    };

    class internal_error final : public std::exception {
        std::string mWhat = "Internal Error";
    public:
        internal_error() = default;

        explicit internal_error(const std::string &str) noexcept:
                mWhat("Internal Error: " + str) {}

        internal_error(const internal_error &) = default;

        internal_error(internal_error &&) noexcept = default;

        ~internal_error() override = default;

        internal_error &operator=(const internal_error &) = default;

        internal_error &operator=(internal_error &&) = default;

        const char *what() const noexcept override
        {
            return this->mWhat.c_str();
        }
    };

    class lang_error final {
        std::string mWhat;
    public:
        lang_error() = default;

        explicit lang_error(std::string str) noexcept:
                mWhat(std::move(str)) {}

        lang_error(const lang_error &) = default;

        lang_error(lang_error &&) noexcept = default;

        ~lang_error() = default;

        lang_error &operator=(const lang_error &) = default;

        lang_error &operator=(lang_error &&) = default;

        const char *what() const noexcept
        {
            return this->mWhat.c_str();
        }
    };

    class fatal_error final : public std::exception {
        std::string mWhat = "Fatal Error";
    public:
        fatal_error() = default;

        explicit fatal_error(const std::string &str) noexcept:
                mWhat("Fatal Error: " + str) {}

        fatal_error(const fatal_error &) = default;

        fatal_error(fatal_error &&) noexcept = default;

        ~fatal_error() override = default;

        fatal_error &operator=(const fatal_error &) = default;

        fatal_error &operator=(fatal_error &&) = default;

        const char *what() const noexcept override
        {
            return this->mWhat.c_str();
        }
    };

    class forward_exception final : public std::exception {
        std::string mWhat;
    public:
        forward_exception() = delete;

        explicit forward_exception(const char *str) noexcept: mWhat(str) {}

        forward_exception(const forward_exception &) = default;

        forward_exception(forward_exception &&) noexcept = default;

        ~forward_exception() override = default;

        forward_exception &operator=(const forward_exception &) = default;

        forward_exception &operator=(forward_exception &&) = default;

        const char *what() const noexcept override
        {
            return this->mWhat.c_str();
        }
    };

    // Static Stack
    template<typename T, std::size_t m_size=1024>
    class stack_type final {
        typename std::aligned_storage<sizeof(T), alignof(T)>::type m_data[m_size];
        T *m_start = nullptr, *m_current = nullptr;
    public:
        class iterator final {
            friend class stack_type;

            T *m_ptr = nullptr;

            explicit iterator(T *const ptr) : m_ptr(ptr) {}

        public:
            iterator() = delete;

            iterator(const iterator &) = default;

            iterator(iterator &&) noexcept = default;

            ~iterator() = default;

            inline T &operator*() const noexcept
            {
                return *m_ptr;
            }

            inline T *operator->() const noexcept
            {
                return m_ptr;
            }

            inline iterator &operator++() noexcept
            {
                --m_ptr;
                return *this;
            }

            inline const iterator operator++(int) noexcept
            {
                return iterator(m_ptr--);
            }

            inline bool operator==(const iterator &it) const noexcept
            {
                return m_ptr == it.m_ptr;
            }

            inline bool operator!=(const iterator &it) const noexcept
            {
                return m_ptr != it.m_ptr;
            }
        };

        stack_type() : m_start(reinterpret_cast<T*>(m_data)), m_current(m_start) {}

        stack_type(const stack_type &) = delete;

        ~stack_type()
        {
            while (m_current != m_start)
                (--m_current)->~T();
        }

        inline bool empty() const
        {
            return m_current == m_start;
        }

        inline std::size_t size() const
        {
            return m_current - m_start;
        }

        inline bool full() const
        {
            return m_current - m_start == m_size;
        }

        inline T &top() const
        {
            if (empty())
                throw cov::error("E000H");
            return *(m_current - 1);
        }

        template<typename...ArgsT>
        inline void push(ArgsT &&...args)
        {
            if (full())
                throw cov::error("E000I");
            ::new(m_current++) T(std::forward<ArgsT>(args)...);
        }

        inline T pop()
        {
            if (empty())
                throw cov::error("E000H");
            --m_current;
            T data(std::move(*m_current));
            m_current->~T();
            return std::move(data);
        }

        inline void pop_no_return()
        {
            if (empty())
                throw cov::error("E000H");
            (--m_current)->~T();
        }

        iterator begin() const noexcept
        {
            return iterator(m_current - 1);
        }

        iterator end() const noexcept
        {
            return iterator(m_start - 1);
        }
    };

    // Buffer Pool
    template<typename T, std::size_t blck_size, template<typename> class allocator_t=std::allocator>
    class allocator_type final {
        T *mPool[blck_size];
        allocator_t<T> mAlloc;
        std::size_t mOffset = 0;
    public:
        allocator_type()
        {
            while (mOffset < 0.5 * blck_size)
                mPool[mOffset++] = mAlloc.allocate(1);
        }

        allocator_type(const allocator_type &) = delete;

        ~allocator_type()
        {
            while (mOffset > 0)
                mAlloc.deallocate(mPool[--mOffset], 1);
        }

        template<typename...ArgsT>
        inline T *alloc(ArgsT &&...args)
        {
            T *ptr = nullptr;
            if (mOffset > 0)
                ptr = mPool[--mOffset];
            else
                ptr = mAlloc.allocate(1);
            mAlloc.construct(ptr, std::forward<ArgsT>(args)...);
            return ptr;
        }

        inline void free(T *ptr)
        {
            mAlloc.destroy(ptr);
            if (mOffset < blck_size)
                mPool[mOffset++] = ptr;
            else
                mAlloc.deallocate(ptr, 1);
        }
    };

    // Binary Tree
    template<typename T>
    class tree_type final {
        struct tree_node final {
            tree_node *root = nullptr;
            tree_node *left = nullptr;
            tree_node *right = nullptr;
            T data;

            tree_node() = default;
            
            tree_node(const tree_node &) = default;

            tree_node(tree_node &&) noexcept = default;

            tree_node(tree_node *a, tree_node *b, tree_node *c, const T &dat) : root(a), left(b), right(c), data(dat) {}

            template<typename...Args_T>
            tree_node(tree_node *a, tree_node *b, tree_node *c, Args_T &&...args):root(a), left(b), right(c),
                                                                                  data(std::forward<Args_T>(args)...) {}

            ~tree_node() = default;
        };

        static tree_node *copy(tree_node *raw, tree_node *root = nullptr)
        {
            if (raw == nullptr) return nullptr;
            tree_node *node = new tree_node(root, nullptr, nullptr, raw->data);
            node->left = copy(raw->left, node);
            node->right = copy(raw->right, node);
            return node;
        }

        static void destroy(tree_node *raw)
        {
            if (raw != nullptr) {
                destroy(raw->left);
                destroy(raw->right);
                delete raw;
            }
        }

        tree_node *mRoot = nullptr;
    public:
        class iterator final {
            friend class tree_type;

            tree_node *mData = nullptr;
        public:
            iterator() = default;

            iterator(tree_node *ptr) : mData(ptr) {}

            iterator(const iterator &) = default;

            iterator(iterator &&) noexcept = default;

            ~iterator() = default;

            iterator &operator=(const iterator &) = default;

            iterator &operator=(iterator &&) noexcept = default;

            bool usable() const noexcept
            {
                return this->mData != nullptr;
            }

            T &data()
            {
                if (!this->usable())
                    throw cov::error("E000E");
                return this->mData->data;
            }

            const T &data() const
            {
                if (!this->usable())
                    throw cov::error("E000E");
                return this->mData->data;
            }

            iterator root() const
            {
                if (!this->usable())
                    throw cov::error("E000E");
                return this->mData->root;
            }

            iterator left() const
            {
                if (!this->usable())
                    throw cov::error("E000E");
                return this->mData->left;
            }

            iterator right() const
            {
                if (!this->usable())
                    throw cov::error("E000E");
                return this->mData->right;
            }
        };

        void swap(tree_type &t)
        {
            tree_node *ptr = this->mRoot;
            this->mRoot = t.mRoot;
            t.mRoot = ptr;
        }

        void swap(tree_type &&t) noexcept
        {
            tree_node *ptr = this->mRoot;
            this->mRoot = t.mRoot;
            t.mRoot = ptr;
        }

        tree_type() = default;

        explicit tree_type(iterator it) : mRoot(copy(it.mData)) {}

        tree_type(const tree_type &t) : mRoot(copy(t.mRoot)) {}

        tree_type(tree_type &&t) noexcept:
                mRoot(nullptr)
        {
            swap(t);
        }

        ~tree_type()
        {
            destroy(this->mRoot);
        }

        tree_type &operator=(const tree_type &t)
        {
            if (&t != this) {
                destroy(this->mRoot);
                this->mRoot = copy(t.mRoot);
            }
            return *this;
        }

        tree_type &operator=(tree_type &&t) noexcept
        {
            swap(t);
            return *this;
        }

        void assign(const tree_type &t)
        {
            if (&t != this) {
                destroy(this->mRoot);
                this->mRoot = copy(t.mRoot);
            }
        }

        bool empty() const noexcept
        {
            return this->mRoot == nullptr;
        }

        void clear()
        {
            destroy(this->mRoot);
            this->mRoot = nullptr;
        }

        iterator root()
        {
            return this->mRoot;
        }

        iterator root() const
        {
            return this->mRoot;
        }

        iterator insert_root_left(iterator it, const T &data)
        {
            if (it.mData == mRoot) {
                mRoot = new tree_node(nullptr, mRoot, nullptr, data);
                return mRoot;
            }
            if (!it.usable())
                throw cov::error("E000E");
            tree_node *node = new tree_node(it.mData->root, it.mData, nullptr, data);
            if (it.mData->root->left == it.mData)
                it.mData->root->left = node;
            else
                it.mData->root->right = node;
            it.mData.root = node;
            return node;
        }

        iterator insert_root_right(iterator it, const T &data)
        {
            if (it.mData == mRoot) {
                mRoot = new tree_node(nullptr, nullptr, mRoot, data);
                return mRoot;
            }
            if (!it.usable())
                throw cov::error("E000E");
            tree_node *node = new tree_node(it.mData->root, nullptr, it.mData, data);
            if (it.mData->root->left == it.mData)
                it.mData->root->left = node;
            else
                it.mData->root->right = node;
            it.mData.root = node;
            return node;
        }

        iterator insert_left_left(iterator it, const T &data)
        {
            if (!it.usable())
                throw cov::error("E000E");
            tree_node *node = new tree_node(it.mData, it.mData->left, nullptr, data);
            if (it.mData->left != nullptr)
                it.mData->left->root = node;
            it.mData->left = node;
            return node;
        }

        iterator insert_left_right(iterator it, const T &data)
        {
            if (!it.usable())
                throw cov::error("E000E");
            tree_node *node = new tree_node(it.mData, nullptr, it.mData->left, data);
            if (it.mData->left != nullptr)
                it.mData->left->root = node;
            it.mData->left = node;
            return node;
        }

        iterator insert_right_left(iterator it, const T &data)
        {
            if (!it.usable())
                throw cov::error("E000E");
            tree_node *node = new tree_node(it.mData, it.mData->right, nullptr, data);
            if (it.mData->right != nullptr)
                it.mData->right->root = node;
            it.mData->right = node;
            return node;
        }

        iterator insert_right_right(iterator it, const T &data)
        {
            if (!it.usable())
                throw cov::error("E000E");
            tree_node *node = new tree_node(it.mData, nullptr, it.mData->right, data);
            if (it.mData->right != nullptr)
                it.mData->right->root = node;
            it.mData->right = node;
            return node;
        }

        template<typename...Args>
        iterator emplace_root_left(iterator it, Args &&...args)
        {
            if (it.mData == mRoot) {
                mRoot = new tree_node(nullptr, mRoot, nullptr, std::forward<Args>(args)...);
                return mRoot;
            }
            if (!it.usable())
                throw cov::error("E000E");
            tree_node *node = new tree_node(it.mData->root, it.mData, nullptr, std::forward<Args>(args)...);
            if (it.mData->root->left == it.mData)
                it.mData->root->left = node;
            else
                it.mData->root->right = node;
            it.mData->root = node;
            return node;
        }

        template<typename...Args>
        iterator emplace_root_right(iterator it, Args &&...args)
        {
            if (it.mData == mRoot) {
                mRoot = new tree_node(nullptr, nullptr, mRoot, std::forward<Args>(args)...);
                return mRoot;
            }
            if (!it.usable())
                throw cov::error("E000E");
            tree_node *node = new tree_node(it.mData->root, nullptr, it.mData, std::forward<Args>(args)...);
            if (it.mData->root->left == it.mData)
                it.mData->root->left = node;
            else
                it.mData->root->right = node;
            it.mData->root = node;
            return node;
        }

        template<typename...Args>
        iterator emplace_left_left(iterator it, Args &&...args)
        {
            if (!it.usable())
                throw cov::error("E000E");
            tree_node *node = new tree_node(it.mData, it.mData->left, nullptr, std::forward<Args>(args)...);
            if (it.mData->left != nullptr)
                it.mData->left->root = node;
            it.mData->left = node;
            return node;
        }

        template<typename...Args>
        iterator emplace_left_right(iterator it, Args &&...args)
        {
            if (!it.usable())
                throw cov::error("E000E");
            tree_node *node = new tree_node(it.mData, nullptr, it.mData->left, std::forward<Args>(args)...);
            if (it.mData->left != nullptr)
                it.mData->left->root = node;
            it.mData->left = node;
            return node;
        }

        template<typename...Args>
        iterator emplace_right_left(iterator it, Args &&...args)
        {
            if (!it.usable())
                throw cov::error("E000E");
            tree_node *node = new tree_node(it.mData, it.mData->right, nullptr, std::forward<Args>(args)...);
            if (it.mData->right != nullptr)
                it.mData->right->root = node;
            it.mData->right = node;
            return node;
        }

        template<typename...Args>
        iterator emplace_right_right(iterator it, Args &&...args)
        {
            if (!it.usable())
                throw cov::error("E000E");
            tree_node *node = new tree_node(it.mData, nullptr, it.mData->right, std::forward<Args>(args)...);
            if (it.mData->right != nullptr)
                it.mData->right->root = node;
            it.mData->right = node;
            return node;
        }

        iterator erase(iterator it)
        {
            if (!it.usable())
                throw cov::error("E000E");
            if (it.mData == mRoot) {
                destroy(mRoot);
                mRoot = nullptr;
                return nullptr;
            }
            tree_node *root = it.mData->root;
            if (root != nullptr) {
                if (it.mData == root->left)
                    root->left = nullptr;
                else
                    root->right = nullptr;
            }
            destroy(it.mData);
            return root;
        }

        iterator reserve_left(iterator it)
        {
            if (!it.usable())
                throw cov::error("E000E");
            tree_node *reserve = it.mData->left;
            tree_node *root = it.mData->root;
            it.mData->left = nullptr;
            reserve->root = root;
            if (root != nullptr) {
                if (it.mData == root->left)
                    root->left = reserve;
                else
                    root->right = reserve;
            }
            destroy(it.mData);
            if (it.mData == mRoot)
                mRoot = reserve;
            return reserve;
        }

        iterator reserve_right(iterator it)
        {
            if (!it.usable())
                throw cov::error("E000E");
            tree_node *reserve = it.mData->right;
            tree_node *root = it.mData->root;
            it.mData->right = nullptr;
            reserve->root = root;
            if (root != nullptr) {
                if (it.mData == root->left)
                    root->left = reserve;
                else
                    root->right = reserve;
            }
            destroy(it.mData);
            if (it.mData == mRoot)
                mRoot = reserve;
            return reserve;
        }

        iterator erase_left(iterator it)
        {
            if (!it.usable())
                throw cov::error("E000E");
            destroy(it.mData->left);
            it.mData->left = nullptr;
            return it;
        }

        iterator erase_right(iterator it)
        {
            if (!it.usable())
                throw cov::error("E000E");
            destroy(it.mData->right);
            it.mData->right = nullptr;
            return it;
        }

        iterator merge(iterator it, const tree_type<T> &tree)
        {
            if (!it.usable())
                throw cov::error("E000E");
            tree_node *root = it.mData->root;
            tree_node *subroot = copy(tree.mRoot, root);
            if (root != nullptr) {
                if (it.mData == root->left)
                    root->left = subroot;
                else
                    root->right = subroot;
            }
            destroy(it.mData);
            if (it.mData == mRoot)
                mRoot = subroot;
            return subroot;
        }
    };

    // Internal Garbage Collection
    template<typename T>
    class garbage_collector final {
        std::forward_list<T *> table_new;
        std::forward_list<T *> table_delete;
    public:
        garbage_collector() = default;

        garbage_collector(const garbage_collector &) = delete;

        ~garbage_collector()
        {
            for (auto &ptr:table_delete)
                table_new.remove(ptr);
            for (auto &ptr:table_new)
                delete ptr;
        }

        void add(void *ptr)
        {
            table_new.push_front(static_cast<T *>(ptr));
        }

        void remove(void *ptr)
        {
            table_delete.push_front(static_cast<T *>(ptr));
        }
    };
}