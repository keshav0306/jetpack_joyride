class Shader{
    public:
    Shader(char * vs_path, char * fs_path);
    Shader();
    void makeProgram(char * vs_path, char * fs_path);
    void use();
    unsigned int vs_id;
    unsigned int fs_id;
    unsigned int prog_id;
    void attatchVs(char * vs_path);
    void attatchFs(char * fs_path);
};