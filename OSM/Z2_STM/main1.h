void FunctionRealTime(int, int, int, int, int *, float);
void DrawAxes(int, int, int, int);
void Cross();
void WaterTank_Pipes(int, int, int, int, float, float, float, float);
void WaterTank(int, int, int, int, float);
int Pipe_In(int, int, int, int, float, int);
void Pipe_Out(int, int, int, int, float, float, float);
void WaterAnimation(int, int, int, int, float, int, int);
float PID_Calculate(float, bool);
float InercModel_Calculate(float);
void clearArray(int *, int);