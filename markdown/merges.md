# 3. merge conflicts

## merge conflicts

- these happen when you merge from different sources:
    - merging
    - rebase
    - pull
    - cherry-pick

## undoing conflict and start over

- Sometimes, you're better off undoing merge conflict:

```bash
git merge --abort # you can abort merge
git rebase --abort # you can abort rebase
```

## understanding git conflict

```
<<<<<<<< HEAD
    <li><a href="index.html">Home</a></li>
    <li><a href="about.html">About us</a></li>
    <li><a href="product.html">Product</a></li>
    <li><a href="imprint.html">Imprint</a></li>
========

>>>>>>>> develop
```

- The top half shows the content in the HEAD branch.
- The bottom is the content of the main branch
    - as you can see, the content is empty.

## merging vs. rebasing

### merge

- when git merges, it looks at t3 commits
    - the common ancestor
    - endpoints of each branch

![alt text](image.png)

- if they have common ancestors, or easy to merge, it'll use an approach known as fast forwarding.
- Otherwise, it'll perform a "knot"

### rebasing

- Here, it'll change the commit history.

- lets say we have two branches, a and b and we need to merge them.

- lets merge b into a.
- a's changes will be "parked"
- b's changes will merge.
- b's previous commits will become the "parked" changed from A.