struct mutex {
  int ref; // reference count
  struct sleeplock lock;
};
