const shm = require('../build/Release/node_shared_memory.node');

module.exports = (data, callback) => {
  const x = JSON.parse(shm.getSharedMemory(__filename, data.size, 1234));
  // console.log(x)
  setTimeout(() => callback(null, 'response'), 0);
  // In the case where the worker returns data, using a second shared memory
  // between workers and farm should be considered.
};
