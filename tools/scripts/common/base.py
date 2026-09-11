# Source: https://gist.github.com/amirsoroush/5f232a1fc7d5a93a244401249fcec83e

def _get_attr(obj, name):
    """Bypass object's normal attribute look up with __getattribute__"""
    return object.__getattribute__(obj, name)

# EDIT (Hyper): renamed class "Super" to "base".
class base:
    __slots__ = "__self", "__type"

    def __init__(self, type_, obj_or_type=None) -> None:
        if not isinstance(type_, type):
            raise TypeError("base() argument 1 must be type.")
        
        if obj_or_type is not None:
            if not (
                isinstance(obj_or_type, type_)
                or (isinstance(obj_or_type, type) and issubclass(obj_or_type, type_))
            ):
                raise TypeError(
                    "base(type, obj): obj must be an instance or subtype of type."
                )

        self.__type = type_
        self.__self = obj_or_type

    @property
    def __thisclass__(self):
        return _get_attr(self, f"_{type(self).__name__}__type")

    @property
    def __self__(self):
        return _get_attr(self, f"_{type(self).__name__}__self")

    @property
    def __self_class__(self):
        __self__ = _get_attr(self, "__self__")

        if __self__ is None:
            return None

        if isinstance(__self__, type):
            return __self__

        __thisclass__ = _get_attr(self, "__thisclass__")
        if isinstance(__self__, __thisclass__):
            return type(_get_attr(self, "__self__"))

    def __get__(self, instance, owner=None):
        """convert unbound base to bound base."""
        if _get_attr(self, "__self__") is None and instance is not None:
            return base(_get_attr(self, "__thisclass__"), instance)
        return self

    def __getattribute__(self, name):
        # Route to properties
        if name in ("__thisclass__", "__self__", "__self_class__"):
            return _get_attr(self, name)

        __self__ = _get_attr(self, "__self__")
        if __self__ is None:
            # This is an unbound base
            return _get_attr(self, name)

        # Delegate to __self__
        mro = __self__.__mro__ if isinstance(__self__, type) else type(__self__).__mro__

        # EDIT (Hyper): changed behaviour to get the base class
        # explicitly by its type, rather than specifying the
        # type to get the base from.
        cls = mro[mro.index(_get_attr(self, "__thisclass__"))]
        if name in cls.__dict__:
            o = cls.__dict__[name]
            if hasattr(o, "__get__") and not isinstance(__self__, type):
                return o.__get__(__self__, _get_attr(self, "__thisclass__"))
            else:
                return o

        raise AttributeError(
            f"'{type(self).__name__}' object has no attribute '{name}'"
        )

    def __repr__(self) -> str:
        type_ = _get_attr(self, "__thisclass__")
        __self__ = _get_attr(self, "__self__")
        if __self__:
            return "<base: <class '{}'>, <{} object>>".format(
                type_.__name__, type(__self__).__name__
            )
        return f"<base: <class '{type_.__name__}'>, NULL>"
