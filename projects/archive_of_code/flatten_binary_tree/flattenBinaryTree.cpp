struct TreeNode {
    int val;
    TreeNode* left;
    TreeNode* right;
};

TreeNode *lastNode(TreeNode *list) {
    if(list == nullptr) return nullptr;
    while(list->right != nullptr) {
        list = list->right;
    }
    return list;
}

TreeNode *flattenTree(TreeNode *root) {
    /* If root is null we already have a DLL :) */
    if(root == nullptr) return nullptr;
    TreeNode *head = root;      // this will be the head of the DLL we return

    /* Recursively convert left and right subtree to a DLL, where
     * the value returned by the recursion is the **HEAD** of
     * the converted DLL.
     */
    TreeNode *left = flattenTree(root->left);
    TreeNode *right = flattenTree(root->right);
    /* We want to combine the left DLL, current node, and right DLL.
     * To do this we need the last node in the left DLL so we can
     * connect it to the current node.
     */
    TreeNode *leftLast = lastNode(left);
    root->left = leftLast;
    /* If the left list isn't empty, then we should set the head of
     * the DLL we are going to return to be the head of the left DLL.
     */
    if(leftLast != nullptr) {
        head = left;
        leftLast->right = root;
    }
    /* Connect current node to the right DLL */
    root->right = right;

    /* Return head of new DLL */
    return head;

}
