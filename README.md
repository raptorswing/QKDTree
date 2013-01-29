QKDTree
=======

A ghetto C++/Qt based kd-tree implementation.

The tree supports storing key/value pairs where the key is a k-dimensional position and the value is anything you can cram into a QVariant.

Insertion (assuming a reasonably balanced tree) can be done in O(logn) time.
Finding the nearest key/value to a key can also be done in O(logn) time (again, assuming reasonably balaned tree).

Tree does not currently support finding the k-nearest key/value pairs to a key but this would be a pretty easy feature to add.
