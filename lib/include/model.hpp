#ifndef MODEL_HPP
#define MODEL_HPP

#include <rocket.hpp>
#include <uuid.h>

template <typename T> class ModelElement
{
 private:
    uuids::uuid mUuid;
    T mElement;

 public:
    rocket::signal<void()> modified;

    ModelElement() = delete;
    ModelElement(const ModelElement&) = delete;
    ModelElement(ModelElement&&) noexcept = default;
    inline explicit ModelElement(const T& element) : mElement(element)
    {
        std::random_device rd;
        auto seed_data = std::array<int, std::mt19937::state_size>{};
        std::ranges::generate(seed_data, std::ref(rd));
        std::seed_seq seq(std::begin(seed_data), std::end(seed_data));
        std::mt19937 generator(seq);
        uuids::uuid_random_generator gen{generator};

        mUuid = gen();
    }

    [[nodiscard]] inline uuids::uuid uuid() const
    {
        return mUuid;
    }

    inline void reset(const T& element)
    {
        mElement = element;
        modified();
    }

    [[nodiscard]] inline const T* operator->() const
    {
        return &mElement;
    }

    [[nodiscard]] inline const T& operator*() const
    {
        return mElement;
    }

    inline bool operator==(const ModelElement&) const = delete;
    ModelElement& operator=(const ModelElement&) = delete;
    ModelElement& operator=(ModelElement&&) noexcept = default;
};

template <typename T> class Model
{
 private:
    std::vector<ModelElement<T>> mElements;
    rocket::scoped_connection_container mElementConnections;

    rocket::signal<void(const uuids::uuid& uuid, const T& element)> elementAdded;
    rocket::signal<void(const uuids::uuid& uuid, const T& element)> elementModified;

 public:
    Model(const Model&) = delete;
    Model() = default;
    Model(Model&&) noexcept = default;

    inline void addElement(const T& element)
    {
        auto& elem = mElements.emplace_back(element);
        mElementConnections += {elem.modified.connect([&elem, this]() { elementModified(elem.uuid(), *elem); })};
        elementAdded(elem.uuid(), *elem);
    }

    [[nodiscard]] inline std::vector<T> elements() const
    {
        std::vector<T> result;
        for (const auto& elem : mElements)
        {
            result.emplace_back(*elem);
        }

        return result;
    }

    inline bool operator==(const Model&) const = delete;
    Model& operator=(const Model&) = delete;
    Model& operator=(Model&&) noexcept = default;
};

#endif // MODEL_HPP
