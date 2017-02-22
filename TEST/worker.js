const util = require('util');
module.exports = (data, callback) => {
  setTimeout(() => callback(null, 'response'), 0);
};
