'use strict';

const path = require('path');
const workerFarm = require('worker-farm');

function promisify(fn) {
  return function() {
    const args = Array.prototype.slice.call(arguments);
    return new Promise((resolve, reject) => {
      args.push((err, res) => {
        if (err) {
          reject(err);
        } else {
          resolve(res);
        }
      });

      fn.apply(this, args);
    });
  };
};

let count = 0;
let total = 0;
const runWithData = data => {
  const farm = workerFarm({
    autoStart: true,
    maxConcurrentCallsPerWorker: 1,
    maxConcurrentWorkers: require('os').cpus().length,
    maxRetries: 2, // Allow for a couple of transient errors.
  }, path.resolve('./worker.js'));
  const f = promisify(farm);
  const run = data => f(data);

  const promises = [];
  const start = Date.now();
  for (let i = 0; i < 1000; i++) {
    promises.push(run(data));
  }

  return Promise.all(promises).then(() => {
    workerFarm.end(farm);
    const end = (Date.now() - start);
    console.log('run ' + (++count) + ' done in ' + end + 'ms');
    total += end;
  });
};

const objects = [{a: 1}];
const object = {};

for (let i = 0; i < 100; i++) {
  const x = {};
  for (let j = 0; j < 10; j++) {
    x[String(Math.random())] = [[[[]]], 5, 3, ['a']];
  }
  object[String(Math.random())] = x;
}
objects.push(object);

let promise = Promise.resolve();

const TIMES = 5;

objects.forEach(object => {
  for (let i = 0; i < TIMES; i++) {
    promise = promise.then(() => runWithData(object));
  }
  promise = promise
    .then(() => {
      console.log('> ' + (total / TIMES) + ' avg');
      console.log('------');
      count = 0;
      total = 0;
    });
});

promise = promise.catch(e => {
  console.log(e);
});
