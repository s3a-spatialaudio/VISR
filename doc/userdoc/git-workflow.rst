VISR Git Workflow
===========================

This page documents the version control workflow followed by the VISR community when dealing with the development of VISR. If you want to contribute, and/or already have code or bug fixes you would like to see in the official repo, please follow these guidelines to make the life of the devs easier, minimize time needed for review, and ensure speedy and efficient incorporation of your improvements into VISR.

VISR Git Workflow
#################

The git workflow for openFrameworks is based on this article detailing a very good branching model. Read the article first, it's quite complete and nicely presented, no need to replicate everything here.

https://nvie.com/posts/a-successful-git-branching-model/

Continuous Integration
######################

All contributions that are merged together in the remote are tested before they are pushed. This is to ensure what you contribute works and is tested on all operating systems. Sometimes, before you push, you might forget to compile or test you code (we all make these mistakes sometimes), so the cvssp-servers at Surrey University will run certain jobs to ensure that your code compiles and is built correcntly.

Every time a merge request from your feature/name-of-your-feature to the remote/origin/develop occurs, a pipeline is triggered testing what you have done before it is merged. You should receive an email if it does not.

On a successful pipeline, other work is done to automatically update VISR' WebAPI. Make sure you comment what you have written!
