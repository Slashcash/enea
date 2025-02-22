#ifndef UTILSLAZY_HPP
#define UTILSLAZY_HPP

/**
 * A lazily loadable class should:
 * - Be default constructible
 * - Provide a isLoaded function returning a bool
 * - Provide a load function
 */
template <typename T>
concept LazyLoadable = std::default_initializable<T> && requires(T obj) {
    { obj.isLoaded() } -> std::convertible_to<bool>;
    { obj.load() };
};

/**
 * This is an helper class that provides lazy load functionalities to classes.
 * By wrapping a lazy loadable class into this template you will get automatic deferred
 * load operation on the object. The object won't get loaded up until the very first moment it is
 * accessed.
 */
template <LazyLoadable T> class Lazy
{
 private:
    T mMember;

 public:
    /**
     * Access the underlying object. If the object was never loaded previously
     * it will be loaded first and then returned.
     */
    [[nodiscard]] inline T& get()
    {
        if (!mMember.isLoaded())
        {
            std::ignore = mMember.load();
        }

        return mMember;
    }
};

#endif // UTILSLAZY_HPP
