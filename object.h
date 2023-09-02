class Object{
    public:
    Object();
    void attatch_vertices(float * vertices, int num_elems); // loc = 0
    void attatch_textures(float * texture, int num_elems); // loc = 2
    void attatch_normals(float * normals, int num_elems); // loc = 1 in vs
    void attatch_indices(unsigned int * indices, int num_elems);
    void attatch_transform(float * transform);
    void attatch_view(float * view);
    void attatch_projection(float * projection);
    void attatch_shader(char * vs_path, char * fs_path);
    void display(int num_triangles);
    unsigned int vao;
    unsigned int vbo[3];
    unsigned int ebo;
    Shader shader;
};